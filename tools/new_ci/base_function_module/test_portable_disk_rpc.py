#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 测试内容：
# 当前版本：1.0 
# 作    者：
# 完成日期：2012/12/26
#*******************************************************************************/
import sys, os
import unittest 


# 公共组件封装
from interface.base_interface.rpc.contrib import tecs_common
from interface.environment_prepare_package import prepare_environment
from interface.config_para import config_para
 
from interface.ssh_advance_package import *
from interface.db_advance_package import *

# 测试接口封装
from interface.base_interface.rpc import vmcfg
from interface.vm_advance_package import *
from interface.image_advance_package import *

class UnitTestPortableDiskCreate(unittest.TestCase):
    '''
    创建移动盘(tecs.portabledisk.create)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        1.上传一个镜像
        2.配置一个虚拟机
        3.虚拟机开机运行
        '''
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        #image_ctrl.check_and_upload_img(self.config_para.server_addr, self.config_para.test_root_user, self.config_para.test_root_passwd, self.config_para.test_image)
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
     #   self.vm_ctrl.add_base("vm1", 1, 1, 256, image_ctrl.get_img_id(), self.environment.get_root_user_project(), self.config_para.storage_loacal_postion)
     #   self.vm_ctrl.add_disk(256, "scsi", "sda", self.config_para.storage_loacal_postion, "ext4")
      #  self.vm_ctrl.add_context("test", "aaaaa")
        # 创建虚拟机
     #   self.vm_ctrl.create()
        # 部署虚拟机
     #   self.vm_ctrl.deploy()
        # 开机
     #   self.vm_ctrl.start()
        # 检测虚拟机为运行态
     #   self.assertTrue(self.db_vm_check_ctrl.vm_state_equal_check_with_time_out(vid, self.config_para.vm_state_run, 60), "wait start fail")
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
        测试步骤：1、创建移动盘
                  2、定时查看SC是否写库完成
        '''
     #   self.vm_ctrl.add_base("vm1", 1, 1, 256, self.image_ctrl.get_img_id(), self.environment.get_root_user_project(), self.config_para.storage_loacal_postion)
      #  request_id = self.vm_ctrl.create_portable_disk(self,256,self.db_vm_check_ctrl.get_cluster_name_by_vid(self.vm_ctrl.get_vm_id("vm1")),"")  
        self.db_vm_check_ctrl.check_pd_storage_cluster()
        request_id = self.vm_ctrl.create_portable_disk(134217728L, "tecscluster", "tecs")
        print "create portable disk request_id=%s" % request_id
        self.assertNotEqual(request_id, False, "create portable disk fail!")
        # 检测新增移动盘相关表是否有对应记录
        self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, 0, 10, 0), "wait create portable disk fail")
        print "pass!!!"
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


class UnitTestPortableDiskDelete(unittest.TestCase):
    '''
    删除移动盘(tecs.portabledisk.delete)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        self.image_ctrl = image_ctrl
        # HC进程运行控制句柄
        self.ssh_tecs_run_ctl = tecs_run_ctl(self.config_para)
        # 虚拟机的SSH检测句柄
        self.ssh_vm_check_ctrl = ssh_vm_check(self.config_para)
        # 虚拟机的db检测句柄
        self.db_vm_check_ctrl = tecs_vm_info_from_db(self.config_para)
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
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
        测试意图：查询所有的虚拟机信息
        测试步骤：1、系统配置了多个虚拟机
                  2、执行查询虚拟机的操作
                  3、检查显示的信息是否完备
                  4、更改显示结果数目
                  5、检查显示的信息是否完备
        '''
        request_id = self.db_vm_check_ctrl.pd_get_request_id()
        print "delete portable disk request_id=%s" % request_id
        self.assertNotEqual(request_id, False, "get request_id false!")
        #删除移动盘
        self.assertTrue(self.vm_ctrl.delete_portable_disk(request_id), "delete portable disk fail!")
        self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, 0, 10, 1), "wait delete portable disk fail")
        print "pass!!!"
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：参数由WEB自动生成查询，暂不编写
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
        测试意图：普通用户查询虚拟机信息
        测试步骤：1、普通用户登录
                  2、执行查询虚拟机的操作
                  3、普通用户可以查询虚拟机信息
                  4、更改显示结果数目
                  5、普通用户可以查询虚拟机信息
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestPortableDiskAttach(unittest.TestCase):
    '''
    attach移动盘(tecs.portabledisk.attach)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        self.image_ctrl = image_ctrl
        # HC进程运行控制句柄
        self.ssh_tecs_run_ctl = tecs_run_ctl(self.config_para)
        # 虚拟机的SSH检测句柄
        self.ssh_vm_check_ctrl = ssh_vm_check(self.config_para)
        # 虚拟机的db检测句柄
        self.db_vm_check_ctrl = tecs_vm_info_from_db(self.config_para)
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        
    def normal_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：附载移动盘到虚拟机
        测试步骤：1、创建一个移动盘
                  2、成功部署一个centos的虚拟机
                  3、attach移动盘到虚拟机
                  4、检查cc的vm_disk表看移动盘是否attach成功
        '''
        vid=self.db_vm_check_ctrl.get_all_deploy_vid()
        request_id = self.db_vm_check_ctrl.pd_get_request_id()
        #request_id = "a1432371-aa91-4b24-88d8-f67ac56052ec"
        print "vid=%ld,request_id=%s" % (long(vid[0]), request_id)
        self.assertNotEqual(request_id, False, "get request_id false!")
        #attach移动盘
        self.assertTrue(self.vm_ctrl.attach_portable_disk(request_id, long(vid[0]), "hdb", "ide"), "attach portable disk fail!")
        self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, long(vid[0]), 30, 2), "wait attach portable disk fail")
        print "pass!!!"
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：参数由WEB自动生成查询，暂不编写
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
        测试意图：普通用户查询虚拟机信息
        测试步骤：1、普通用户登录
                  2、执行查询虚拟机的操作
                  3、普通用户可以查询虚拟机信息
                  4、更改显示结果数目
                  5、普通用户可以查询虚拟机信息
        '''
        pass;
    
    def test_entery(self):
        # 测试入口
        self.prepare_private_environment()
        self.normal_test_level_2()
        self.Exception_test_level_2()
        self.limit_test_level_2()
        self.auth_test_level_2()

class UnitTestPortableDiskDetach(unittest.TestCase):
    '''
    attach移动盘(tecs.portabledisk.attach)
    '''  
    def __init__(self, environment):
        self.environment = environment
        self.config_para = self.environment.config_para
        
    def prepare_private_environment(self):
        '''
        测试私有环境准备，只在该文件中有效，在下个文件中，就无效了！,该文件是RPC接口测试文件，请不要把虚拟机部署
        的准备工作放到这里，如果放到这里，需要在该RPC接口测试结束的时候，进行手动的销毁！以保证不影响其他RPC接口测试
        '''
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        self.image_ctrl = image_ctrl
        # HC进程运行控制句柄
        self.ssh_tecs_run_ctl = tecs_run_ctl(self.config_para)
        # 虚拟机的SSH检测句柄
        self.ssh_vm_check_ctrl = ssh_vm_check(self.config_para)
        # 虚拟机的db检测句柄
        self.db_vm_check_ctrl = tecs_vm_info_from_db(self.config_para)
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        pass;

# 正常测试
    def normal_test_level_2(self):
        '''
        正常测试
        '''
        self.normal_test_level_3_1()
        
    def normal_test_level_3_1(self):
        '''
        是否实现：是
        测试意图：从虚拟机detach一个移动盘
        测试步骤：1、选择一个带移动盘的虚拟机
                  2、detach移动盘
        '''
        request_id, vid = self.db_vm_check_ctrl.pd_get_attach_request_id_and_vid()
        print "vid=%ld,request_id=%s" % (long(vid), request_id)
        self.assertNotEqual(request_id, False, "get request_id and vid false!")
        #attach移动盘
        self.assertTrue(self.vm_ctrl.detach_portable_disk(long(vid), request_id), "detach portable disk fail!")
        self.assertTrue(self.db_vm_check_ctrl.pd_state_equal_check_with_time_out(request_id, long(vid), 30, 3), "wait detach portable disk fail")
        print "pass!!!"
        pass;
        
# 异常流程测试
    def Exception_test_level_2(self):
        '''
        异常测试
        '''
        self.Exception_test_level_3_1()
        
    def Exception_test_level_3_1(self):
        '''
        是否实现：否
        测试意图：参数由WEB自动生成查询，暂不编写
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
        测试意图：普通用户查询虚拟机信息
        测试步骤：1、普通用户登录
                  2、执行查询虚拟机的操作
                  3、普通用户可以查询虚拟机信息
                  4、更改显示结果数目
                  5、普通用户可以查询虚拟机信息
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
    #UnitTestPortableDiskCreate(environment).test_entery()
    UnitTestPortableDiskDelete(environment).test_entery()
    #UnitTestPortableDiskAttach(environment).test_entery()
    #UnitTestPortableDiskDetach(environment).test_entery()

# 模块文档输出总入口
def all_doc_entry(cidoc):
    # 保存在EXCEL中的sheet标签名称定义
    module_name = "移动盘操作 RPC"
    #cidoc.write_doc(UnitTestVmcfgFullQuery, module_name)
    #cidoc.write_doc(UnitTestVmcfgGetIdByName, module_name)
    #cidoc.write_doc(UnitTestVmcfgQuery, module_name)
    #cidoc.write_doc(UnitTestVmcfgQueryRelation, module_name)
    #cidoc.write_doc(UnitTestVmcfgQueryVmSynState, module_name)
    #cidoc.write_doc(UnitTestVmcfgRelationSet, module_name)
    #cidoc.write_doc(UnitTestVmcfgAction, module_name)
    #cidoc.write_doc(UnitTestVmcfgAllocate, module_name)



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
