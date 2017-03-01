#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：host.py
# 测试内容：host模块xmlrpc接口封装
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
#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
START_INDEX = 0L
QUERY_COUNT = 200L
#主机状态
INIT = 0
OFF_LINE = 1
ON_LINE = 2
HOST_ENABLE = bool(1)
HOST_DISABLE = bool(0)
#****************************************************************************#

class host:
    ''' tecs host methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session

    def disable(self,clustername,hostname,is_forced):
        result = self.apisvr.tecs.host.disable(self.session,clustername,hostname,is_forced) 
        if result[0] == 0:
            print "disable host %s in cluster %s success!" % (hostname,clustername)
            return True
        else:
            print "failed to disable host %s in cluster %s! error code = %s" % (hostname,clustername,result[0])
            print "error string = %s" % result[1]
            return False
            
    def enable(self,clustername,hostname):
        result = self.apisvr.tecs.host.enable(self.session,clustername,hostname) 
        if result[0] == 0:
            print "enbale host %s in cluster %s success!" % (hostname,clustername)
            return True
        else:
            print "failed to enbale host %s in cluster %s! error code = %s" % (hostname,clustername,result[0])
            print "error string = %s" % result[1]
            return False

    def forget(self,clustername,hostname):
        result = self.apisvr.tecs.host.forget(self.session,clustername,hostname) 
        if result[0] == 0:
            print "forget %s in cluster %s success!" % (hostname,clustername)
            return True
        else:
            print "failed to forget host %s in cluster %s! error code = %s" % (hostname,clustername,result[0])
            print "error string = %s" % result[1]
            return False
            
    def query(self,start_index,query_count,cluster_name,host_name):
        result = self.apisvr.tecs.host.query(self.session,start_index,query_count,cluster_name,host_name) 
        if result[0] != 0:
            print "query host %s in cluster %s information failed! error code =%d" % (host_name,cluster_name,result[0]) 
            return False
        else:
            if len(result[3]) == 0:
                print "No host ",host_name
                if host_name == "":
                    return True
                else:
                    return False
                    
            print "host information:" 
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            host_list = result[3]
            print "---------host list---------" 
            for hc in host_list:
                print "host %s" % hc["oid"]
                for k in hc.keys():
                    print "  %s = %s" % (k,hc[k])
            return True    

    def reboot(self,clustername,hostname):
        result = self.apisvr.tecs.host.reboot(self.session,clustername,hostname) 
        if result[0] == 0:
            print "reboot host %s in cluster %s success!" % (hostname,clustername)
            return True
        else:
            print "failed to reboot host %s in cluster %s! error code = %s" % (hostname,clustername,result[0])
            print "error string = %s" % result[1]
            return False    

    def set(self,clustername,hostname,description):
        result = self.apisvr.tecs.host.set(self.session,clustername,hostname,description) 
        if result[0] == 0:
            print "register host %s to cluster %s success!" % (hostname,clustername)
            return True
        else:
            print "failed to register host %s to cluster %s! error code = %s" % (hostname,clustername,result[0])
            print "error string = %s" % result[1]
            return False


    def shutdown(self,clustername,hostname):
        result = self.apisvr.tecs.host.shutdown(self.session,clustername,hostname) 
        if result[0] == 0:
            print "shutdown host %s in cluster %s success!" % (hostname,clustername)
            return True
        else:
            print "failed to shutdown host %s in cluster %s! error code = %s" % (hostname,clustername,result[0])
            print "error string = %s" % result[1]
            return False

#根据指定的主机名查询其cpu id            
    def get_cpu_id(self,cluster_name,host_name):
        if host_name is None:
            return None
        result = self.apisvr.tecs.host.query(self.session,START_INDEX,QUERY_COUNT,cluster_name,host_name) 
        if result[0] != 0:
            print "query host %s in cluster %s information failed! error code =%d" % (host_name,cluster_name,result[0]) 
            return None
        host_list = result[3]
        if len(host_list) != 1:
            return None
        return  host_list[0]["cpu_info_id"]
        
#执行host的外部控制命令        
    def host_extern_ctrl_cmd(self, cluster_name, host_name,ctrl_cmd,arg_info = ""):
        if host_name is None:
            return None
        result = self.apisvr.tecs.host.extern_ctrl_cmd(self.session, cluster_name,host_name,ctrl_cmd,arg_info)
        if result[0] != 0 :
            print "extern cmd %s in cluster %s host %s failed! info %s" % (ctrl_cmd,cluster_name, host_name , result[1])
        else :
            print "extern cmd %s in cluster %s host %s sucess!" % (ctrl_cmd,cluster_name, host_name )
    def host_extern_ctrl_query(self, cluster_name, host_name, arg_info = ""):
        if host_name is None:
            return None
        result = self.apisvr.tecs.host.extern_ctrl.query(self.session, cluster_name, host_name, arg_info)
        if result[0] != 0 :
            print "extern query in cluster %s host %s failed! info %s" % (cluster_name, host_name, result[1])
        else :
            print "extern query in cluster %s host %s sucess: state %d" %(cluster_name, host_name, result[1])
    
    def host_extern_ctrl_cfg_set(self, cluster_name, host_name, node_type, node_manager, node_address) :
        if host_name is None :
        	return None
        result = self.apisvr.tecs.host.extern_ctrl_cfg_set(self.session, cluster_name, host_name, node_type, node_manager, node_address) 
        if result[0] != 0 :
            print "extern cfg set in cluster %s host %s  failed! info %s" % (cluster_name, host_name, result[1])
        else :
            print "extern cfg set in cluster %s host %s  sucess! " % (cluster_name, host_name)
   
    def host_extern_ctrl_cfg_get(self, cluster_name, host_name) :
        if host_name is None:
        	return None
        result = self.apisvr.tecs.host.extern_ctrl_cfg_get(self.session, cluster_name, host_name) 
        if result[0] != 0 :
            print "extern cfg get in cluster %s host %s  failed! info %s" % (cluster_name, host_name, result[1])
        else :
            print "extern cfg get in cluster %s host %s  sucess! info type %s, manager %s, address %s" % (cluster_name, host_name, result[1],result[2],result[3])
   
                
    def query_value(self,start_index,query_count,cluster_name,host_name,query_name):
        result = self.apisvr.tecs.host.query(self.session,start_index,query_count,cluster_name,host_name) 
        if result[0] != 0:
            print "query host %s in cluster %s information failed! error code =%d" % (host_name,cluster_name,result[0]) 
            return None
        else:                   
            print "host information:" 
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            host_list = result[3]
            print "---------host list---------" 
            for hc in host_list:
                print "host %s" % hc["oid"]
                for k in hc.keys():
                    print "  %s = %s" % (k,hc[k])
            for hc in host_list:
                return  hc[query_name]
            return None  

    def query_run_state(self,cluster_name,host_name):             
        return self.query_value(START_INDEX,QUERY_COUNT,cluster_name,host_name,"run_state")

    def query_enable_state(self,cluster_name,host_name):             
        return self.query_value(START_INDEX,QUERY_COUNT,cluster_name,host_name,"is_disabled")

    def query_running_vms(self,cluster_name,host_name):             
        return self.query_value(START_INDEX,QUERY_COUNT,cluster_name,host_name,"running_vms")            
                
                
                
                