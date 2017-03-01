#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：test_1.py
# 测试内容：clustermanager模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
#*******************************************************************************/
from rpc import tecs_rpc

#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
START_INDEX = 0L
QUERY_COUNT = 200L
#query mode
ASYN_MODE = 0
SYN_MODE = 1

CM_DISABLE = 0
CM_ENABLE = 1

IS_OFFLINE = 0
IS_ONLINE = 1
#****************************************************************************#

class clustermanager:
    ''' tecs clustermanager methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session
        
    def disable(self,clustername):
        result = self.apisvr.tecs.cluster.disable(self.session,clustername) 
        if result[0] == 0:
            print "disable cluster %s success!" % clustername
            return True
        else:
            print "failed! error code = %s" % result[0]
            print "error string = %s" % result[1]
            return False   
    
    def enable(self,clustername):
        result = self.apisvr.tecs.cluster.enable(self.session,clustername) 
        if result[0] == 0:
            print "enable cluster %s success!" % clustername
            return True
        else:
            print "failed! error code = %s" % result[0]
            print "error string = %s" % result[1]
            return False 
    
    def query(self,start_index,query_count,mode):
        result = self.apisvr.tecs.cluster.query(self.session,start_index,query_count,mode) 
        if result[0] != 0:
            print "query cluster information failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return False
        else:
            print "cluster information:" 
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            cluster_list = result[3]
            print "---------cluster list---------"  
            for cluster in cluster_list:
                print "cluster %s" % cluster["cluster_name"]
                for k in cluster.keys():
                    print "  %s = %s" % (k,cluster[k])
            return True

    def get_fisrt_clustername(self):
        '''根据指定的集群名称查询其注册状态  '''
        result = self.apisvr.tecs.cluster.query(self.session,START_INDEX,QUERY_COUNT,1) 
        if result[0] != 0:
            print "query cluster information failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            max_count = long(result[2])
            if max_count == 0:
                return None
            print "query cluster information succesed!max_count=%d" % max_count
            cm_list = result[3]
            for cm in cm_list:
                if cm["cluster_name"] != None:
                    print "fisrt_clustername is %s" % cm["cluster_name"] 
                    return cm["cluster_name"]
    
    def get_cluster_max_count(self):
        '''根据指定的集群名称和模式查询其注册状态  '''
        result = self.apisvr.tecs.cluster.query(self.session,START_INDEX,QUERY_COUNT,1) 
        if result[0] != 0:
            print "query cluster information failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:            
            max_count = long(result[2])
            return max_count
             
    def get_is_online(self,clustername,mode):
        '''根据指定的集群名称和模式查询其注册状态  '''
        result = self.apisvr.tecs.cluster.query(self.session,START_INDEX,QUERY_COUNT,mode) 
        if result[0] != 0:
            print "query cluster information failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:            
            max_count = long(result[2])
            if max_count == 0:
               print "query cluster information succesed!max_count=%d." % max_count
               return None
            cm_list = result[3]
            for c in cm_list:
                if c["cluster_name"] == clustername:
                    print "get is_online success. is_online=%d" % int(c["is_online"])
                    return int(c["is_online"])
            print "query cluster information succesed!no cluster info."                    
            return None
            
    def get_enabled(self,clustername,mode):
        '''根据指定的集群名称查询其注册状态  '''
        result = self.apisvr.tecs.cluster.query(self.session,START_INDEX,QUERY_COUNT,mode) 
        if result[0] != 0:
            print "query cluster information failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            max_count = long(result[2])
            if max_count == 0:
               print "query cluster information succesed!max_count=%d." % max_count
               return None
            cm_list = result[3]
            for c in cm_list:
                if c["cluster_name"] == clustername:
                    print "get enabled success. enabled=%d" % int(c["enabled"])
                    return int(c["enabled"])
            print "query cluster information succesed!no cluster info."                    
            return None    

    def query_core_tcu(self,mode):
        result = self.apisvr.tecs.cluster.query_core_tcu(self.session) 
        if result[0] != 0:
            print "query cluster core_tcu information failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return False
        else:
            print "cluster core_tcu information:" 
            print "core_max_num = %d" % result[1]
            print "tcu_max_num = %d" % result[2]
            if mode ==0:
                return True
            elif mode ==1:
                return int(result[1])
            elif mode ==2:
                return int(result[2])
    
    def get_core_num(self):
        return int(self.query_core_tcu(1))
    
    def get_tcu_num(self):
        return int(self.query_core_tcu(2))    
          
    def query_netplane(self,clustername,mode):
        result = self.apisvr.tecs.cluster.query_netplane(self.session,clustername) 
        if result[0] != 0:
            print "query cluster net_plan information failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return False
        else:
            print "cluster core_tcu information:" 
            print "%s's net_plan is %s" % (clustername,result[0])
            if mode ==0:
                return True
            elif mode ==1:
                return result[0]
    
    def get_netplane(self,clustername):
        return self.query_netplane(clustername,1)
        
    def register(self,clustername,info):
        result = self.apisvr.tecs.cluster.register(self.session,clustername,info) 
        if result[0] == 0:
            print "register cluster %s success!" % clustername
            return True
        else:
            print "failed! error code = %s" % result[0]
            print "error string = %s" % result[1]
            return False
            
    def set(self,clustername,info):
        result = self.apisvr.tecs.cluster.set(self.session,clustername,info) 
        if result[0] == 0:
            print "set cluster %s success!" % clustername
            return True
        else:
            print "failed! error code = %s" % result[0]
            print "error string = %s" % result[1]
            return False    
    
    def forget(self,clustername):
        result = self.apisvr.tecs.cluster.forget(self.session,clustername) 
        if result[0] == 0:
            print "forget cluster %s success!" % clustername
            return True
        else:
            print "failed! error code = %s" % result[0]
            print "error string = %s" % result[1]
            return False
    
    
                
                
                
                
                
                
                
                
                