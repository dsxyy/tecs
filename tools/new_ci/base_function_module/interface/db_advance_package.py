#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：ssh_advance_package.py
# 测试内容：测试环境的数据库恢复和备份方法
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/12/24
#*******************************************************************************/
import sys,os,getopt,time
from base_interface.ssh_cmd import *
from base_interface.rpc.contrib.tecs_common import *
from base_interface.postgresql import *

# TECS虚拟机类
class tecs_vm_info_from_db:
    def __init__(self,config_para):
        self.config_para = config_para
        
    def __del__(self):
        pass;
    
    # 获取TC内所有虚拟机的vid,下面的db_ctrl 不能优化到init里面去，否则在最外面使用的时候，会存在DB数据库有链接的时候，进行数据库恢复出现异常
    def get_all_deploy_vid(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        vid = []
        cluster_name = db_ctrl.query_from_tc("cluster_pool", "name", "1=1")
        for cluster in cluster_name:
            vid.extend(db_ctrl.query_from_cc(cluster, "vm_pool", "vid", "1=1"))
        return vid
    
    # 通过vid来获取所在集群的名称    
    def get_cluster_name_by_vid(self, vid):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        names = db_ctrl.query_from_tc("vmcfg_pool", "deployed_cluster", "oid=%d" %long(vid))
        if len(names) == 0:
            return None
        return names[0]
    
    # 通过虚拟机名称查找第一个符合名称要求的虚拟机
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
            
    # 在指定时间内，判断虚拟机cancel表是否清NULL了,cluster那么不能在里面取，因为cancel成功后，tc就没有记录         
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
    
    # 内部函数，还要被超时等封装，不做过多的非法判断    
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
        
        
    # 在指定的时间内，判断上次虚拟机操作是否成功了    
    def check_vm_op_success_with_time_out(self, vid, time_limit):
        if self.check_vm_op_result_time_out(vid, time_limit) == "Success":
            return True
        return False
    
    # 在指定的时间内获取虚拟机的状态，首先要判断当前操作已经结束，如果没有结束，不会获取状态    
    def vm_state_equal_check_with_time_out(self, vid, state, time_limit):
        if self.check_vm_op_success_with_time_out(vid, time_limit) == False:
            return False
        return self.__check_vm_state_with_time_out(vid, state, time_limit)
    # 根据不同的操作类型标识返回结果
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
    # 在指定的时间内，判断上次移动盘操作是否成功了
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
    # 在指定的时间内获取移动盘的状态    
    def pd_state_equal_check_with_time_out(self, request_id, vid, time_limit, flag):
        if self.check_pd_op_result_time_out(request_id, vid, time_limit, flag) == False:
            return False
        return True
        
    # 随机构造磁阵和cluster的映射关系，供测试使用    
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
    
    # 检查是否存在磁阵和cluster的映射关系
    def check_pd_storage_cluster(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        clusters = db_ctrl.query_from_tc("storage_cluster", "cluster_name", "1=1")
        if len(clusters) != 0:
            return clusters[0]
        ret = self.pd_storage_cluster_map()
        if ret != False:
            return ret
            
    # 随机获取一个移动盘的request_id
    def pd_get_request_id(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        requestid = db_ctrl.query_from_tc("storage_user_volume", "request_id", "1=1")
        if len(requestid) != 0:
            return requestid[0]
        return False

    # 随机获取一个已经attach的移动盘的request_id和vid
    def pd_get_attach_request_id_and_vid(self):
        db_ctrl = postgresql_db(self.config_para.server_addr, "5432", "tecs", "tecs", self.config_para.test_cloud)
        requestid = db_ctrl.query_from_cc("tecscluster","vm_disk", "request_id", "request_id != ''")
        if len(requestid) != 0:
            vid = db_ctrl.query_from_cc("tecscluster","vm_disk", "vid", "request_id = '%s'" % requestid[0])
            return requestid[0],vid[0]
        return "",-1


    #部署虚拟机后，检查CC上(记录比较长，更新比较频繁)的表空间占用最多的10张表
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
        
    
