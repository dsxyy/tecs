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
from base_interface.rpc.contrib.tecs_common import *



class config_para:
    def __init__(self):
        #���ڲ��Ե�tc xmlrpc��������ַ
        self.server_addr = "localhost"
        #���ڲ��Ե�tc xmlrpc�������˿ں�
        self.server_port = "8080"
        # tc/cc/hc��������Ҫ��start.sh�еĶ��屣��һ�£�
        #���ڲ��Ե�tc application����������
        self.test_cloud = "tecscloud"
        #���ڲ��Ե�cc application������Ⱥ����
        self.test_cluster = "tecscluster"
        #���ڲ��Ե�hc application�������������
        self.test_host = "localhost"
        #����Ա�˺ź�����
        self.test_root_user = "admin"
        self.test_root_passwd = "admin"
        #��ͨ�û�������
        self.test_usergroup = "cigroup"
        #�⻧�˺ź�����
        self.test_tenant_user = "citest"
        self.test_tenant_passwd = "citest"
        #�����ϴ��������������ӳ���ļ�·��
        self.test_image = "ttylinux.img"
        #�����Ե����������
        self.test_vm_name = "ttylinux"
        #�����ԵĹ�������
        self.test_project_name = "test_prj_1"
        # ssh��¼����
        self.test_host_ssh_pwd = "ossdbg1"
        
        # ����һЩ����ϵͳ�ĳ���*******************start**********************
        # �洢���ֵĳ���
        # ��Ӧ����� STORAGE_POSITION_LOCAL
        self.storage_loacal_postion = 1
        # ��Ӧ����� STORAGE_POSITION_SHARE
        self.storage_share_postion = 2
        
        # �����״̬�ĳ���
        # ��Ӧ����� VM_RUNNING
        self.vm_state_run = "2"
        # ��Ӧ����� VM_PAUSED
        self.vm_state_pause = "3"
        # ��Ӧ����� VM_SHUTOFF
        self.vm_state_shutoff = "4"
        # ��Ӧ����� VM_CRASHED
        self.vm_state_crash= "5"
        # ��Ӧ����� VM_UNKNOWN
        self.vm_state_unknown = "6"
        # ��Ӧ����� VM_CONFIGURATION
        self.vm_state_config = "7"
        
        # ������������������ֵ����
        # ��Ӧ����� SUCCESS
        self.vm_op_result_success = "0"
        # ��Ӧ����� ERROR_OP_RUNNING
        self.vm_op_result_running = "33"
        # ����һЩ����ϵͳ�ĳ���*******************end**********************

    def usage(self):
        ''' ��ӡ������Ϣ'''
        print "Usage: %s [option]" % os.path.basename(sys.argv[0])
        print " -a,--addr, the xmlrpc server ip address or host name"
        print " -p,--port, the xmlrpc server port"
        print " -i,--image, the vm image file to upload"
        sys.exit(0)

    def get_config(self, argv):
        # �������������в���
        print argv
        try:
            opts, args = getopt.getopt(argv, "h:a:p:i:n:v", ["help", "addr=","port=","image=","hcname="])
        except getopt.GetoptError:
            # print help information and exit:
            self.usage()
            ci_exit(-2,"GetoptError")
            
        for o, v in opts:
            #��-h��--help��ӡ����
            if o in ("-h", "--help"):
                self.usage()
                ci_exit(0)
            #��-a��--addr����ָ��tc��xmlrpc��������ַ��Ĭ��Ϊlocalhost
            if o in ("-a", "--addr"):
                self.server_addr = v
            #��-p��--port����ָ��tc��xmlrpc�������˿ںţ�Ĭ��Ϊ8080
            if o in ("-p", "--port"):
                self.server_port = v
            #ָ�����������ļ�����������
            if o in ("-n", "--hcname"):
                self.test_host = v
            #��-i��--image����ָ����������õ�ӳ���ļ�
            if o in ("-i", "--image"):
                self.test_image = v
                if os.path.isfile(self.test_image) is not True:
                    ci_exit(-2,"image file %s not exist!" % self.test_image)        

        


 

