#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_001.py
# �������ݣ����������������������
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/03/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/03/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
# �޸ļ�¼2��
#    �޸����ڣ�2012/07/13
#    �� �� �ţ�V1.0
#    �� �� �ˣ�gaom
#    �޸����ݣ�EC611003077308 tecs�ṩrpc�ӿڲ�ѯϵͳ����ʱ��Ϣ
#*******************************************************************************/
import sys,os,getopt,time

sys.path.append("/opt/tecs/tc/cli") 

# ��������ͷģ��
from base_function_module.interface.config_para import config_para
from base_function_module.interface.environment_prepare_package import *

# ����ʹ�õ��ĸ߼���װ����
from base_function_module.interface.image_advance_package import image_module
from base_function_module.interface.vm_advance_package import vm_module
from base_function_module.interface.vmtemplate_advance_package import vmt_module


class Demo:
    def __init__(self, environment):
        self.environment = environment
        self.config_para = environment.config_para
        
        # ��Ҫ�����Ĺ�������
        demo_project_name = "demo_project"
        environment.check_and_get_normal_user(self.config_para.test_tenant_user, self.config_para.test_tenant_passwd, self.config_para.test_usergroup)
        environment.check_and_get_project(demo_project_name)
    
    def prepare_img(self):
        # demoʹ�õ�img , �û�������Ҫ�޸�
        demo_image = "/home/tecs_install/ttylinux0.img"
        # ��ȡ��imageģ��Ĺ������
        image_ctrl = image_module(self.config_para.server_addr, self.environment.get_root_session())
        image_ctrl.check_and_upload_img(self.config_para.server_addr, self.config_para.test_root_user, self.config_para.test_root_passwd, demo_image)
        self.image_ctrl = image_ctrl
        # ����ӳ��
        self.image_ctrl.publish()

    # ���������ģ��
    def create_vmtemplate(self):
        # ��ȡ��������Ĺ������
        vmt_ctrl = vmt_module(self.config_para.server_addr, self.environment.get_root_session())
        
        # �����������������
        test_vmt_name = "ttylinux"
        # ������������         ���������,�����CPU��,TCU��,�ڴ��С(��λMByte),img_id,��������,img�����ϵͳ�е�λ�� 
        vmt_ctrl.add_base(test_vmt_name, 1, 1, 256, self.image_ctrl.get_img_id(), self.config_para.storage_loacal_postion)
        
        # ���Ӵ���
        # �����������壺     ���̴�С(��λM), �����������ͣ��������ƣ����̴�ŵ�λ�ã����̸�ʽ������
        vmt_ctrl.add_disk(256, "scsi", "sda", self.config_para.storage_loacal_postion, "ext4")
        # ������һ�����̣�д������
        vmt_ctrl.add_disk(512, "scsi", "sdb", self.config_para.storage_loacal_postion, "ext4")
        
        # ����������
        # �����������壺  �������ļ����ƣ�����������
        vmt_ctrl.add_context("file_name1", "file_value1")
        # ������һ�������ĸ�ʽ���£�
        vmt_ctrl.add_context("file_name2", "file_value2")
        
        #��������������,�����������ã���Ҫ��TECSϵͳ����������ƽ�棬�����������������ã���Ҫ����֮ǰ������������ƽ��
        # �������壺           ����ƽ������, �Ƿ�ʹ��DHCP(0���ܣ�1ʹ��)��
        # vmt_ctrl.add_nic(netplane,     1)
        
        # ����һ��ģ��
        vmt_ctrl.create()
    
    # ���������    
    def create_vm(self):
        # ��ȡ��������Ĺ������
        self.vm_ctrl = vm_module(self.config_para.server_addr, self.environment.get_root_session())
        # ����������Ļ�������
        # �����������������
        test_vm_name = "ttylinux"
        # ������������         ���������,�����CPU��,TCU��,�ڴ��С(��λMByte),img_id,��������,img�����ϵͳ�е�λ�� 
        self.vm_ctrl.add_base(test_vm_name, 1, 1, 256, self.image_ctrl.get_img_id(), self.environment.get_root_user_project(), self.config_para.storage_loacal_postion)
        
        # ���Ӵ���
        # �����������壺     ���̴�С(��λM), �����������ͣ��������ƣ����̴�ŵ�λ�ã����̸�ʽ������
        self.vm_ctrl.add_disk(128, "scsi", "sda", self.config_para.storage_loacal_postion, "ext4")
        # ������һ�����̣�д������
        self.vm_ctrl.add_disk(512, "scsi", "sdb", self.config_para.storage_loacal_postion, "ext4")
        
        # ����������
        # �����������壺  �������ļ����ƣ�����������
        self.vm_ctrl.add_context("file_name1", "file_value1")
        # ������һ�������ĸ�ʽ���£�
        self.vm_ctrl.add_context("file_name2", "file_value2")
        
        #��������������,�����������ã���Ҫ��TECSϵͳ����������ƽ�棬�����������������ã���Ҫ����֮ǰ������������ƽ��
        # �������壺           ����ƽ������, �Ƿ�ʹ��DHCP(0���ܣ�1ʹ��)��
        # self.vm_ctrl.add_nic(netplane,     1)
        
        # ����һ�������
        self.vm_ctrl.create()
        # ����һ�������
        self.vm_ctrl.deploy()
        
        # ����벿����������ظ�������������
        #self.vm_ctrl.create()
        #self.vm_ctrl.deploy()
        

    def create_demo(self):
        self.prepare_img()
        self.create_vmtemplate()
        self.create_vm()

# ��������ִ�����λ��
if __name__ == "__main__":
        file_name = sys.argv[0]
        # ��ȡ���ò����ľ��
        config_para_handler = config_para()
        config_para_handler.get_config(sys.argv[1:])
        # ��ȡ����׼���������������ʼ��
        environment = prepare_environment(config_para_handler)
        environment.wait_sys_ok()
        # ִ��Demo
        Demo(environment).create_demo()

