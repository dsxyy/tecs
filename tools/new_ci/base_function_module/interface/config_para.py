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
from base_interface.rpc.contrib.tecs_common import *



class config_para:
    def __init__(self):
        #用于测试的tc xmlrpc服务器地址
        self.server_addr = "localhost"
        #用于测试的tc xmlrpc服务器端口号
        self.server_port = "8080"
        # tc/cc/hc的名字需要和start.sh中的定义保持一致！
        #用于测试的tc application，即云名称
        self.test_cloud = "tecscloud"
        #用于测试的cc application，即集群名称
        self.test_cluster = "tecscluster"
        #用于测试的hc application，即物理机名称
        self.test_host = "localhost"
        #管理员账号和密码
        self.test_root_user = "admin"
        self.test_root_passwd = "admin"
        #普通用户组名称
        self.test_usergroup = "cigroup"
        #租户账号和密码
        self.test_tenant_user = "citest"
        self.test_tenant_passwd = "citest"
        #用于上传，创建虚拟机的映像文件路径
        self.test_image = "ttylinux.img"
        #待测试的虚拟机名称
        self.test_vm_name = "ttylinux"
        #待测试的工程名称
        self.test_project_name = "test_prj_1"
        # ssh登录密码
        self.test_host_ssh_pwd = "ossdbg1"
        
        # 定义一些测试系统的常量*******************start**********************
        # 存储部分的常量
        # 对应代码宏 STORAGE_POSITION_LOCAL
        self.storage_loacal_postion = 1
        # 对应代码宏 STORAGE_POSITION_SHARE
        self.storage_share_postion = 2
        
        # 虚拟机状态的常量
        # 对应代码宏 VM_RUNNING
        self.vm_state_run = "2"
        # 对应代码宏 VM_PAUSED
        self.vm_state_pause = "3"
        # 对应代码宏 VM_SHUTOFF
        self.vm_state_shutoff = "4"
        # 对应代码宏 VM_CRASHED
        self.vm_state_crash= "5"
        # 对应代码宏 VM_UNKNOWN
        self.vm_state_unknown = "6"
        # 对应代码宏 VM_CONFIGURATION
        self.vm_state_config = "7"
        
        # 定义虚拟机操作结果的值定义
        # 对应代码宏 SUCCESS
        self.vm_op_result_success = "0"
        # 对应代码宏 ERROR_OP_RUNNING
        self.vm_op_result_running = "33"
        # 定义一些测试系统的常量*******************end**********************

    def usage(self):
        ''' 打印帮助信息'''
        print "Usage: %s [option]" % os.path.basename(sys.argv[0])
        print " -a,--addr, the xmlrpc server ip address or host name"
        print " -p,--port, the xmlrpc server port"
        print " -i,--image, the vm image file to upload"
        sys.exit(0)

    def get_config(self, argv):
        # 解析处理命令行参数
        print argv
        try:
            opts, args = getopt.getopt(argv, "h:a:p:i:n:v", ["help", "addr=","port=","image=","hcname="])
        except getopt.GetoptError:
            # print help information and exit:
            self.usage()
            ci_exit(-2,"GetoptError")
            
        for o, v in opts:
            #用-h或--help打印帮助
            if o in ("-h", "--help"):
                self.usage()
                ci_exit(0)
            #用-a或--addr可以指定tc的xmlrpc服务器地址，默认为localhost
            if o in ("-a", "--addr"):
                self.server_addr = v
            #用-p或--port可以指定tc的xmlrpc服务器端口号，默认为8080
            if o in ("-p", "--port"):
                self.server_port = v
            #指定测试配置文件的主机名称
            if o in ("-n", "--hcname"):
                self.test_host = v
            #用-i或--image可以指定虚拟机所用的映像文件
            if o in ("-i", "--image"):
                self.test_image = v
                if os.path.isfile(self.test_image) is not True:
                    ci_exit(-2,"image file %s not exist!" % self.test_image)        

        


 

