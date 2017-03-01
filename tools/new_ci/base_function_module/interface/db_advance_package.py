#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�ssh_advance_package.py
# �������ݣ����Ի��������ݿ�ָ��ͱ��ݷ���
# ��ǰ�汾��1.0 
# ��    �ߣ���Т��
# ������ڣ�2012/12/24
#*******************************************************************************/
import sys,os,getopt,time
from base_interface.ssh_cmd import *
from base_interface.rpc.contrib.tecs_common import *
from base_interface.postgresql import *

# TECS�������
class tecs_vm_info_from_db:
    def __init__(self,config_para):
        self.config_para = config_para
        
    def __del__(self):
        pass;
    
    # ��ȡTC�������������vid,�����db_ctrl �����Ż���init����ȥ��������������ʹ�õ�ʱ�򣬻����DB���ݿ������ӵ�ʱ�򣬽������ݿ�ָ������쳣
    def get_all_deploy_vid(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        vid = []
        cluster_name = db_ctrl.query_from_tc("cluster_pool", "name", "1=1")
        for cluster in cluster_name:
            vid.extend(db_ctrl.query_from_cc(cluster, "vm_pool", "vid", "1=1"))
        return vid
    
    # ͨ��vid����ȡ���ڼ�Ⱥ������    
    def get_cluster_name_by_vid(self, vid):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        names = db_ctrl.query_from_tc("vmcfg_pool", "deployed_cluster", "oid=%d" %long(vid))
        if len(names) == 0:
            return None
        return names[0]
    
    # ͨ����������Ʋ��ҵ�һ����������Ҫ��������
    def get_vmid_by_vm_name(self, vm_name):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        vids = db_ctrl.query_from_tc("vmcfg_pool", "oid", "name='%s'" % str(vm_name))
        if len(vids) == 0:
            return -1
        return vids[0]
    
        
    def check_vm_op_result_time_out(self, vid, time_limit):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        time_count = 0
        cluster_name = self.get_cluster_name_by_vid(vid)
        if cluster_name == None:
            return "Fail"
        state = db_ctrl.query_from_cc(cluster_name, "web_view_vmstate", "last_op_result", "vid=%d" % long(vid))
        if len(state) == 0:
            return "NULL"
        while state[0] == self.config_para.vm_op_result_running:
            time.sleep(2)
            time_count += 2
            if time_count >= time_limit:
                return "TimeOut"
            state = db_ctrl.query_from_cc(cluster_name, "web_view_vmstate", "last_op_result", "vid=%d" % long(vid))
        if state[0] == self.config_para.vm_op_result_success:
            return "Success"
        else:
            return "Faill"
            
    # ��ָ��ʱ���ڣ��ж������cancel���Ƿ���NULL��,cluster��ô����������ȡ����Ϊcancel�ɹ���tc��û�м�¼         
    def check_vm_cancel_success_with_time_out(self, cluster_name, vid, time_limit):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        state = db_ctrl.query_from_cc(cluster_name, "vm_cancel_pool", "oid", "vid=%d" % long(vid))
        time_count = 0
        while len(state) != 0:
            time_delay(2, "check db item not exist from cc")
            time_count += 2
            if time_count >= time_limit:
                return False
            state = db_ctrl.query_from_cc(cluster_name, "vm_cancel_pool", "oid", "vid=%d" % long(vid))
        return True
    
    # �ڲ���������Ҫ����ʱ�ȷ�װ����������ķǷ��ж�    
    def __check_vm_state_with_time_out(self, vid, state_d, time_limit):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        cluster_name = self.get_cluster_name_by_vid(vid)
        state = db_ctrl.query_from_cc(cluster_name, "web_view_vmstate", "state", "vid=%d" % long(vid))
        time_count = 0
        while state[0] != state_d:
            time_delay(2, "check vm state ,need wait")
            time_count += 2
            if time_count > time_limit:
                return False
            state = db_ctrl.query_from_cc(cluster_name, "web_view_vmstate", "state", "vid=%d" % long(vid))
        return True
        
        
    # ��ָ����ʱ���ڣ��ж��ϴ�����������Ƿ�ɹ���    
    def check_vm_op_success_with_time_out(self, vid, time_limit):
        if self.check_vm_op_result_time_out(vid, time_limit) == "Success":
            return True
        return False
    
    # ��ָ����ʱ���ڻ�ȡ�������״̬������Ҫ�жϵ�ǰ�����Ѿ����������û�н����������ȡ״̬    
    def vm_state_equal_check_with_time_out(self, vid, state, time_limit):
        if self.check_vm_op_success_with_time_out(vid, time_limit) == False:
            return False
        return self.__check_vm_state_with_time_out(vid, state, time_limit)
    # ���ݲ�ͬ�Ĳ������ͱ�ʶ���ؽ��
    def check_pd_op_result(self, exist, flag):
        if flag == 0 or flag == 2:
            if exist == 0:
                return True
            else:
                return False
        else:
            if exist != 0:
                return True
            else:
                return False
    # ��ָ����ʱ���ڣ��ж��ϴ��ƶ��̲����Ƿ�ɹ���
    def check_pd_op_result_time_out(self, request_id, vid, time_limit, flag):
        time_count = 0
        cluster_name = ""
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        if flag == 0 or flag == 1:
            names = db_ctrl.query_from_tc("storage_user_volume", "uid", "request_id='%s'" % str(request_id))
        else:
            cluster_name = self.get_cluster_name_by_vid(vid)
            names = db_ctrl.query_from_cc(cluster_name,"vm_disk", "vid", "request_id='%s'" % str(request_id))
        while self.check_pd_op_result(len(names), flag):
            time.sleep(2)
            time_count += 2
            if time_count >= time_limit:
                return False
            if flag == 0 or flag == 1:
                names = db_ctrl.query_from_tc("storage_user_volume", "uid", "request_id='%s'" % str(request_id))
            else:
                names = db_ctrl.query_from_cc(cluster_name,"vm_disk", "vid", "request_id='%s'" % str(request_id))
            if self.check_pd_op_result(len(names), flag):
                continue
            else:
                return True
    # ��ָ����ʱ���ڻ�ȡ�ƶ��̵�״̬    
    def pd_state_equal_check_with_time_out(self, request_id, vid, time_limit, flag):
        if self.check_pd_op_result_time_out(request_id, vid, time_limit, flag) == False:
            return False
        return True
        
    # �����������cluster��ӳ���ϵ��������ʹ��    
    def pd_storage_cluster_map(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        sids = db_ctrl.query_from_tc("storage_adaptor", "sid", "1=1")
        if len(sids) == 0:
            print "storage_adaptor is empty, cannot create portable disk!" 
            return False
        clusters = db_ctrl.query_from_tc("cluster_pool", "name", "1=1")
        if len(clusters) == 0:
            print "cluster_pool is empty, cannot create portable disk!" 
            return False
        ret = db_ctrl.add_to_storage_cluster(long(sids[0]),clusters[0])
        if ret == True:
            return clusters[0]
        print "add_to_storage_cluster False, cannot create portable disk! ret = %d!"  % ret
        return False
    
    # ����Ƿ���ڴ����cluster��ӳ���ϵ
    def check_pd_storage_cluster(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        clusters = db_ctrl.query_from_tc("storage_cluster", "cluster_name", "1=1")
        if len(clusters) != 0:
            return clusters[0]
        ret = self.pd_storage_cluster_map()
        if ret != False:
            return ret
            
    # �����ȡһ���ƶ��̵�request_id
    def pd_get_request_id(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        requestid = db_ctrl.query_from_tc("storage_user_volume", "request_id", "1=1")
        if len(requestid) != 0:
            return requestid[0]
        return False

    # �����ȡһ���Ѿ�attach���ƶ��̵�request_id��vid
    def pd_get_attach_request_id_and_vid(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        requestid = db_ctrl.query_from_cc("tecscluster","vm_disk", "request_id", "request_id != ''")
        if len(requestid) != 0:
            vid = db_ctrl.query_from_cc("tecscluster","vm_disk", "vid", "request_id = '%s'" % requestid[0])
            return requestid[0],vid[0]
        return "",-1


    #����������󣬼��CC��(��¼�Ƚϳ������±Ƚ�Ƶ��)�ı�ռ�ռ������10�ű�
    def pd_get_table_size_info(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        tabname = "pg_stat_user_tables"
        column = "pg_relation_size(relid) as tablesize,schemaname,relname,n_live_tup"
        condition = "1=1 order by tablesize desc  limit 10"
        
        results = db_ctrl.query_from_cc_ex("tecscluster", tabname, column, condition)
        print  "----------------------------------------"
        for row in results:
            for r in row:
                print r,
            print "\n"
        print  "----------------------------------------"
        
    
