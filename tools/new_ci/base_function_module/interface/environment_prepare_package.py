#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�test_014_vm.py
# �������ݣ�
# ��ǰ�汾��1.0 
# ��    �ߣ���Т��
# ������ڣ�2012/11/27
# ˵��    ������ӿ���Ҫ֧�֣��������ݿ��Ƿ����������ݣ�����Ҫִ�гɹ�
#*******************************************************************************/
import sys,os,getopt,time,inspect
from ftplib import FTP

from ssh_advance_package import *
from db_advance_package import *
from vm_advance_package import *

from base_interface.rpc.usergroup import *
from base_interface.rpc.user import *
from base_interface.rpc.tecssystem  import *
from base_interface.rpc.host  import *
from base_interface.rpc.tcu  import *
from base_interface.rpc.project  import *
from base_interface.rpc.rpc import *
from base_interface.postgresql import *
from base_interface.rpc.contrib.tecs_common import *



class prepare_environment:
    def __init__(self, config_para): 
        self.config_para = config_para
        self.rpc = tecs_rpc(config_para.server_addr)
        self.host = config_para.server_addr
        self.server = self.rpc.server    
#        self.root_session = login(self.server, config_para.test_root_user, config_para.test_root_passwd)
#        if self.root_session == None:
#            ci_exit(-1, "use admin login fail")
    
    def refresh_root_session(self, pwd):
        self.root_session = login(self.server,self.config_para.test_root_user,pwd)
        if self.root_session == None:
            ci_exit(-1, "use admin login fail")
        
    def wait_tc_ok(self):
        sys_runtime_query = tecssystem(self.host,self.root_session)
        i = 0
        while i < 40:
            print "waiting tc startup"
            if sys_runtime_query.get_runtime_state_range_ok(TYPE_CACULATE,DEFAULT_TC, RUNTIME_STATE_NORMAL):
                print "tc startup success"
                break
            i = i+1 
            time_delay(2, "wait tc ok")  
        if i >= 20:
            ci_exit(-1, "tc status is abnormal")
    
    # ��ȡ����CC������״̬�Ƿ�����    
    def wait_all_cc_ok(self):
        # �Ȼ�ȡ��ǰ���е�HC��������
        db = postgresql_db("127.0.0.1", "5432", "tecs", "tecs", "tecscloud")
        clusters_name = db.query_from_tc("cluster_pool", "name", "1=1")
        self.all_cluster=clusters_name
        cluster_num = 0
        wait_count = 0
        for cluster_name in clusters_name:
            self.wait_cc_ok(cluster_name)
            cluster_num = cluster_num + 1
        # ���һ��CC��û�У������²�ѯ�ȴ�
        if cluster_num == 0:
            wait_count = wait_count + 1
            # ���CC��60S��û�б����֣���ֱ�Ӵ����˳�
            if wait_count >= 12:
                ci_exit(-1, "find cc fail")
            time_delay(5, "wait all cc ok") 
            self.wait_all_cc_ok()
        
    def wait_cc_ok(self, cc_name):
        sys_runtime_query = tecssystem(self.host,self.root_session)
        i = 0
        while i < 40:
            print "waiting cc: %s startup" % cc_name
            if sys_runtime_query.get_runtime_state_range_ok(TYPE_CACULATE,cc_name, RUNTIME_STATE_NORMAL):
                print"cc: %s startup success" % cc_name
                break
            i = i+1 
            time_delay(2, "wait cc ok")  
        if i >= 20:
            ci_exit(-1, "tc status is abnormal")

    # ��TC�ϻ�ȡ����CC�Ƿ�������״̬
    def wait_all_cc_online(self):
        db = postgresql_db("127.0.0.1", "5432", "tecs", "tecs", "tecscloud")
        onlines = db.query_from_tc("cluster_pool", "is_online", "1=1")
        for online in onlines:
            if int(online) != 1:
                return False
        return True
        
    def wait_all_cc_online_timeout(self, timeout_value):
        wait_time = 0
        while self.wait_all_cc_online() != True:
            time.sleep(2)
            wait_time = wait_time + 2
            if wait_time >= timeout_value:
                ci_exit(-1, "wait_all_cc_online time out")
                    
    # ��ȡ����HC������״̬�Ƿ���ȷ
    def wait_all_hc_ok(self):
        self.all_hosts = {}
        # �Ȼ�ȡ��ǰ���е�HC��������
        db = postgresql_db("127.0.0.1", "5432", "tecs", "tecs", "tecscloud")
        for cc_name in self.all_cluster:
            hosts_name = db.query_from_cc(cc_name, "host_pool", "name", "1=1")
            self.all_hosts[cc_name] = hosts_name
            for host_name in hosts_name:
                self.wait_hc_ok(cc_name, host_name)

    def wait_hc_ok(self, cc_name, hc_name):
        print hc_name 
        test_cc_hc = cc_name + "," + hc_name
        sys_runtime_query = tecssystem(self.host,self.root_session)
        i = 0
        while i < 40:
            print "waiting hc: %s/%s startup" % (cc_name, hc_name)
            if sys_runtime_query.get_runtime_state_range_ok(TYPE_CACULATE,test_cc_hc, RUNTIME_STATE_WARNNING):
                print "hc: %s/%s startup success" % (cc_name, hc_name)
                break
            i = i+1 
            time_delay(2, "wait hc ok")  
        if i >= 20:
            ci_exit(-1, "tc status is abnormal")

    def wait_all_hc_online(self):
        db = postgresql_db("127.0.0.1", "5432", "tecs", "tecs", "tecscloud")
        for cluster in self.all_cluster:
            run_status = db.query_from_cc(cluster ,"host_pool", "run_state", "1=1")
            for statu in run_status:
                if int(statu) != 2:
                    return False
        return True
            
    def wait_all_hc_online_timeout(self, timeout_value):
        wait_time = 0
        while self.wait_all_hc_online() != True:
            time.sleep(2)
            wait_time = wait_time + 2
            if wait_time >= timeout_value:
                ci_exit(-1, "wait_all_hc_online time out")
        
        
# ����һ���û�������û����ڣ����¼���û�����ȡ���û���session
    def check_and_get_normal_user(self, user_name, user_pwd, user_group):
        ug  = usergroup(self.host,self.root_session)
        if ug.query(START_INDEX, QUERY_COUNT, user_group) == False:
            if ug.allocate(user_group,GROUP_TYPE1) == False:
                ci_exit(-1, "create user_group fail")
                
        u = user(self.host, self.root_session)
        if u.query(START_INDEX, QUERY_COUNT, user_name) == False:
            if u.allocate(user_name,user_pwd,user_pwd,ACCOUNTUSER,user_group) == False:
                ci_exit(-1, "create nomal user fail")
        
        self.user1_session = login(self.server,user_name,user_pwd)
        self.user_name = user_name
        
    def wait_host_discover(self, cc_name, hc_name):
        root_host_method = host(self.host,self.root_session)
        i = 0
        while i < 10: 
            print "waiting host: %s/%s discover" % (cc_name, hc_name)
            if root_host_method.query(START_INDEX,QUERY_COUNT,test_cluster,test_host) == TRUE:
                print "discover: %s/%s  success" % (cc_name, hc_name)
                break
            i = i+1 
            time.sleep(2) 
        if i >= 10:
            ci_exit(-1, "discover host fail")
    
    def set_tcu(self, cc_name, hc_name, tcu_num):
        root_host_method = host(self.host,self.root_session)
        cpu_id = root_host_method.get_cpu_id(cc_name,hc_name)
        if cpu_id == None:
            ci_exit(-1, "cpu id find fail")
        print "cpu id of host %s = %d" % (hc_name,cpu_id) 
        
        root_tcu_method = tcu(self.host,self.root_session)
        if root_tcu_method.set(cc_name,cpu_id,tcu_num,"test cpu description") == False:
            ci_exit(-1, "set tcu fail")
      
    def set_tcu_default(self, cc_name, tcu_num):
        cpu_id = 1L
        root_tcu_method = tcu(self.host,self.root_session)
        if root_tcu_method.set(cc_name,cpu_id,tcu_num,"test cpu description") == False:
            ci_exit(-1, "set tcu fail")
      
    def check_and_get_project(self, test_project_name="test_prj_1", test_project_name2="test_prj_2"):
        user_project = project(self.host,self.user1_session)
        if user_project.query_count(START_INDEX,QUERY_COUNT,QUERY_BY_PROJECT_NAME,test_project_name,self.user_name) == None:
            if user_project.allocate(test_project_name,"test project description") == False:
                ci_exit(-1, "create project fail")

        if user_project.query_count(START_INDEX,QUERY_COUNT,QUERY_BY_PROJECT_NAME,test_project_name2,self.user_name) == None:
            if user_project.allocate(test_project_name2,"test project2 description") == False:
                ci_exit(-1, "create project fail")
        
        root_project = project(self.host,self.root_session)
        if root_project.query_count(START_INDEX,QUERY_COUNT,QUERY_BY_PROJECT_NAME,test_project_name,self.config_para.test_root_user) == None:
            if root_project.allocate(test_project_name,"test project description") == False:
                ci_exit(-1, "create project fail")
        
        self.root_user_project = test_project_name
        self.user_project = test_project_name
        self.user_project2 = test_project_name2
    
    # ��ȡһ�����������µĲ��������ID����ȫ�°�װ�������棬��ȡ����ID��-1
    def get_upgrade_test_vm(self):
        db_ctrl = tecs_vm_info_from_db(self.config_para)
        # ���ҷ������Ƶ������ID
        # ���������õ����������Ϊ��"upgrade_test_vm1"
        return db_ctrl.get_vmid_by_vm_name("upgrade_test_vm1")
        
    def get_user_session(self):
        return self.user1_session
        
    def get_root_session(self):
        return self.root_session
        
    def get_root_user_project(self):
        return self.root_user_project
        
    def get_nomal_user_project(self):
        return self.user_project
        
    def get_nomal_user2_project2(self):
        return self.user_project2     

    def get_all_host_name(self):
        return self.all_hosts
        
    def get_all_cluster_name(self):
        return self.all_cluster
        
    def wait_sys_ok(self):
        self.refresh_root_session(self.config_para.test_root_passwd)
        self.wait_tc_ok()
        self.wait_all_cc_ok()
        self.wait_all_cc_online_timeout(60)
        self.wait_all_hc_ok()
        self.wait_all_hc_online_timeout(60)
    
    def prepare_public_environment(self):
        self.check_and_get_normal_user(self.config_para.test_tenant_user, self.config_para.test_tenant_passwd, self.config_para.test_usergroup)
        self.check_and_get_project()
        self.set_tcu_default("tecscluster", 10)

# ��������������ID֮������������
def clean_vm(environment, save_vid):
    # ��ȡ��ǰ�����������ڵ������
    current_vid = tecs_vm_info_from_db(environment.config_para).get_all_deploy_vid()
    print "clean vm which deployed in testing..."
    print "current vid is :";print current_vid
    print "save vid is :";print save_vid
    # �������������������͵�ǰ��������һ�£���������
    if len(current_vid) == len(save_vid):
        return
    # ��ȡVM��RPC���ƾ��
    vm_module_handler = vm_module(environment.config_para.server_addr, environment.get_root_session())  
    db_ctrl = tecs_vm_info_from_db(environment.config_para)    
    for vid in current_vid:
        if save_vid.count(vid) == 0:
            print "will clean vm %s" % vid
            # ��ʼִ��������������ȡ��
            vm_module_handler.set_vmid(vid)
            cluster_name = db_ctrl.get_cluster_name_by_vid(vid)
            vm_module_handler.cancel()
            # ���������Ƿ�cancel�ɹ�
            db_ctrl.check_vm_cancel_success_with_time_out(cluster_name, long(vid), 30)

# ˽�п��յĽ���            
class private_snapshot:
    def __init__(self, environment, private_run_env):
        # ��ȡһ������׼������
        self.environment = environment
        self.config_para = self.environment.config_para
        # ��ȡ�������п��ƾ�����ں���ʹ��
        self.local_run_ctrl = tecs_run_ctl(self.config_para)
        # ��ȡ�������ݿ���ƽű����,���������ݿ��ļ�
        self.local_ssh_pg_db = ssh_pg_db_ctl(self.config_para)
        self.local_ssh_pg_db.back_tc_db("private", self.config_para.test_cloud)
        self.local_ssh_pg_db.back_cc_db("private", self.config_para.test_cluster)
        # �Ǽ������е������,�����ָ���ʱ����Щ�������������ɾ���ָ�
        self.save_vid = tecs_vm_info_from_db(self.config_para).get_all_deploy_vid()
        # ��¼����������л���׼�������Ļص�����
        self.private_run_env = private_run_env


    def recover(self): 
        # ����TECS
        self.local_run_ctrl.start_all_server()
        # �ȴ�ϵͳ�ϵ�
        self.environment.wait_sys_ok()
        # �������������
        clean_vm(self.environment, self.save_vid)
        # ֹͣ����
        self.local_run_ctrl.stop_all_server()
        # �ָ����ݿ�
        self.local_ssh_pg_db.recover_tc_db("private", self.config_para.test_cloud)
        self.local_ssh_pg_db.recover_cc_db("private", self.config_para.test_cluster)
        # ����TECS
        self.local_run_ctrl.start_all_server()
        # �ȴ�ϵͳ�ϵ�
        self.environment.wait_sys_ok()
        
        
# �����������Ի����ľ���        
class public_snapshot:
    def __init__(self, config_para):
        self.config_para = config_para
        #����ȡ���������ļ����������ȡ�����ļ�
        self.local_cfg_file = tecs_cfg_file(config_para)
        # ���û�������־����ȼ�
        self.updata_tc_config("exc_log_level", 2)  
        self.updata_cc_config("exc_log_level", 2) 
        self.updata_hc_config("exc_log_level", 2)
        # ���������ļ�
        self.local_cfg_file.back_tc_cfg()
        self.local_cfg_file.back_cc_cfg()
        self.local_cfg_file.back_hc_cfg()
        # ��ȡ�������п��ƾ�����ں���ʹ��
        self.local_run_ctrl = tecs_run_ctl(config_para)
        # ��ȡ�������ݿ���ƽű����,���������ݿ��ļ�
        self.local_ssh_pg_db = ssh_pg_db_ctl(config_para)
        self.local_ssh_pg_db.back_tc_db("public", config_para.test_cloud)
        self.local_ssh_pg_db.back_cc_db("public", config_para.test_cluster)
        # �Ǽ������е������,�����ָ���ʱ����Щ�������������ɾ���ָ�
        self.save_vid = tecs_vm_info_from_db(config_para).get_all_deploy_vid()
        # ��ȡһ������׼������
        self.environment = prepare_environment(config_para)
        
    def recover(self): 
        # ����TECS
        self.local_run_ctrl.start_all_server()
        # �ȴ�ϵͳ�ϵ�
        self.environment.wait_sys_ok()
        # �������������
        clean_vm(self.environment, self.save_vid)
        # ֹͣ����
        self.local_run_ctrl.stop_all_server()
        # �ָ������ļ�
        self.local_cfg_file.recover_tc_cfg()
        self.local_cfg_file.recover_cc_cfg()
        self.local_cfg_file.recover_hc_cfg()
        # �ָ����ݿ�
        self.local_ssh_pg_db.recover_tc_db("public", self.config_para.test_cloud)
        self.local_ssh_pg_db.recover_cc_db("public", self.config_para.test_cluster)
        
    def updata_tc_config(self, key, value):
        self.local_cfg_file.updata_tc_config(key, value) 
        
    def updata_cc_config(self, key, value):
        self.local_cfg_file.updata_cc_config(key, value) 
        
    def updata_hc_config(self, key, value):
        self.local_cfg_file.updata_hc_config(key, value) 
        
    def start(self):
        self.local_run_ctrl.start_all_server()
        # �ȴ�ϵͳ�ϵ�
        self.environment.wait_sys_ok()