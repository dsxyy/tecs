#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_001.py
# 测试内容：测试虚拟机创建部署流程
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
# 修改记录2：
#    修改日期：2012/07/13
#    版 本 号：V1.0
#    修 改 人：gaom
#    修改内容：EC611003077308 tecs提供rpc接口查询系统运行时信息
#*******************************************************************************/
import sys,os,getopt,time

sys.path.append("/opt/tecs/tc/cli") 

# 基本引用头模块
from base_function_module.interface.config_para import config_para
from base_function_module.interface.environment_prepare_package import *

# 测试使用到的高级封装引用
from base_function_module.interface.image_advance_package import image_module
from base_function_module.interface.vm_advance_package import vm_module
from base_function_module.interface.vmtemplate_advance_package import vmt_module


class Demo:
    def __init__(self, environment):
        self.environment = environment
        self.config_para = environment.config_para
        
        # 需要创建的工程名称
        demo_project_name = "demo_project"
        environment.check_and_get_normal_user(self.config_para.test_tenant_user, self.config_para.test_tenant_passwd, self.config_para.test_usergroup)
        environment.check_and_get_project(demo_project_name)
    
    def prepare_img(self):
        # demo使用的img , 用户根据需要修改
        demo_image = "/home/tecs_install/ttylinux0.img"
        # 获取到image模块的公共句柄
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        image_ctrl.check_and_upload_img(self.config_para.server_addr, self.config_para.test_root_user, self.config_para.test_root_passwd, demo_image)
        self.image_ctrl = image_ctrl
        # 发布映像
        self.image_ctrl.publish()

    # 创建虚拟机模板
    def create_vmtemplate(self):
        # 获取到虚拟机的公共句柄
        vmt_ctrl = vmt_module(self.config_para.server_addr, self.environment.get_root_session())
        
        # 待创建虚拟机的名称
        test_vmt_name = "ttylinux"
        # 各参数的意义         虚拟机名称,虚拟机CPU数,TCU数,内存大小(单位MByte),img_id,工程名称,img存放在系统中的位置 
        vmt_ctrl.add_base(test_vmt_name, 1, 1, 256, self.image_ctrl.get_img_id(), self.config_para.storage_loacal_postion)
        
        # 增加磁盘
        # 各参数的意义：     磁盘大小(单位M), 磁盘总线类型，磁盘名称，磁盘存放的位置，磁盘格式化类型
        vmt_ctrl.add_disk(256, "scsi", "sda", self.config_para.storage_loacal_postion, "ext4")
        # 再增加一个磁盘，写法如下
        vmt_ctrl.add_disk(512, "scsi", "sdb", self.config_para.storage_loacal_postion, "ext4")
        
        # 增加上下文
        # 各参数的意义：  上下文文件名称，上下文内容
        vmt_ctrl.add_context("file_name1", "file_value1")
        # 再增加一个上下文格式如下：
        vmt_ctrl.add_context("file_name2", "file_value2")
        
        #下面是网卡配置,由于网卡配置，需要在TECS系统中增加网络平面，所以如果想打开下面配置，需要在这之前，先配置网络平面
        # 参数意义：           网络平面名称, 是否使能DHCP(0禁能，1使能)，
        # vmt_ctrl.add_nic(netplane,     1)
        
        # 创建一个模板
        vmt_ctrl.create()
    
    # 创建虚拟机    
    def create_vm(self):
        # 获取到虚拟机的公共句柄
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        # 配置虚拟机的基本配置
        # 待创建虚拟机的名称
        test_vm_name = "ttylinux"
        # 各参数的意义         虚拟机名称,虚拟机CPU数,TCU数,内存大小(单位MByte),img_id,工程名称,img存放在系统中的位置 
        self.vm_ctrl.add_base(test_vm_name, 1, 1, 256, self.image_ctrl.get_img_id(), self.environment.get_root_user_project(), self.config_para.storage_loacal_postion)
        
        # 增加磁盘
        # 各参数的意义：     磁盘大小(单位M), 磁盘总线类型，磁盘名称，磁盘存放的位置，磁盘格式化类型
        self.vm_ctrl.add_disk(128, "scsi", "sda", self.config_para.storage_loacal_postion, "ext4")
        # 再增加一个磁盘，写法如下
        self.vm_ctrl.add_disk(512, "scsi", "sdb", self.config_para.storage_loacal_postion, "ext4")
        
        # 增加上下文
        # 各参数的意义：  上下文文件名称，上下文内容
        self.vm_ctrl.add_context("file_name1", "file_value1")
        # 再增加一个上下文格式如下：
        self.vm_ctrl.add_context("file_name2", "file_value2")
        
        #下面是网卡配置,由于网卡配置，需要在TECS系统中增加网络平面，所以如果想打开下面配置，需要在这之前，先配置网络平面
        # 参数意义：           网络平面名称, 是否使能DHCP(0禁能，1使能)，
        # self.vm_ctrl.add_nic(netplane,     1)
        
        # 创建一个虚拟机
        self.vm_ctrl.create()
        # 部署一个虚拟机
        self.vm_ctrl.deploy()
        
        # 如果想部署多个虚拟机重复下面两个函数
        #self.vm_ctrl.create()
        #self.vm_ctrl.deploy()
        

    def create_demo(self):
        self.prepare_img()
        self.create_vmtemplate()
        self.create_vm()

# 单个函数执行体的位置
if __name__ == "__main__":
        file_name = sys.argv[0]
        # 获取配置参数的句柄
        config_para_handler = config_para()
        config_para_handler.get_config(sys.argv[1:])
        # 获取环境准备公共句柄，并初始化
        environment = prepare_environment(config_para_handler)
        environment.wait_sys_ok()
        # 执行Demo
        Demo(environment).create_demo()

