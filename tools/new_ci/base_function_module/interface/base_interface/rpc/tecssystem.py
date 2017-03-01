#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：system.py
# 测试内容：system运行时信息查询管理模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/07/07
#
# 修改记录1：
#    修改日期：2012/07/07
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
#*******************************************************************************/
from rpc import tecs_rpc

#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
TYPE_CACULATE = "caculate"
DEFAULT_TC = ""
RUNTIME_STATE_NORMAL = 0
#RUNTIME_STATE_NOTICE = 1 
RUNTIME_STATE_WARNNING = 2
#RUNTIME_STATE_ERROR = 3 
#****************************************************************************#

class tecssystem:
    ''' tecs system methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session
      
    def show_runtime_state(self,nodetype,target):
        '''查询指定节点的运行时信息'''
        result = self.apisvr.tecs.system.runtime.query(self.session,nodetype,target) 
        if result[0] != 0:
            print "failed to query system runtime state! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            appinfo=result[1]
            print "==============tecs runtime information=============="
            print "application = %s" % appinfo["name"]
            print "query at: %s" % appinfo["collect_time"]
            print "runtime state: %d" % appinfo["runtime_state"]
            for proc in appinfo["procinfo"]:
                print "=======process: %s" % proc["name"]
                if proc["exist"] == True:
                    print "runtime state: %d" % proc["runtime_state"]
                    print "pid: %d" % proc["pid"]
                    print "running_seconds: %d" % proc["running_seconds"]
                    print "exceptions: %d" % proc["exceptions"]
                    for mu in proc["muinfo"]:
                        if mu["exist"] == True:
                            print "---mu: %s" % mu["name"]
                            print "runtime_state: %d" % mu["runtime_state"]
                            print "state: %d" % mu["state"]
                            print "age: %d" % mu["age"]
                            print "pid: %d" % mu["pid"]
                            print "push_count: %d" % mu["push_count"]
                            print "pop_count: %d" % mu["pop_count"]
                            print "queued_count: %d" % mu["queued_count"]
                            print "timer_count: %d" % mu["timer_count"]
                            print "last_message: %d" % mu["last_message"]
                        else:
                            print "mu %s not exist!" % mu["name"]
                else:
                    print "process not exist!"

    def get_runtime_state(self,nodetype,target):
        '''获取指定节点的运行时状态'''
        result = self.apisvr.tecs.system.runtime.query(self.session,nodetype,target) 
        if result[0] != 0:
            print "failed to query system runtime state! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return -1
        else:
            appinfo=result[1]            
            return appinfo["runtime_state"]
            
    def get_runtime_state_range_ok(self, nodetype,target, value):
        run_state = int(self.get_runtime_state(nodetype,target))
        if run_state >= RUNTIME_STATE_NORMAL and run_state <= value:
            return True
        self.show_runtime_state(nodetype,target)
        return True
            
    def show_license(self):
        '''查询许可证信息'''
        result = self.apisvr.tecs.system.license.query(self.session) 
        if result[0] != 0:
            print "failed to query license information! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            license=result[1]
            print "================license================"
            print "customer_name = %s" % license["customer_name"]
            print "customer_company = %s" % license["customer_company"]
            print "customer_address = %s" % license["customer_address"]
            print "customer_phone = %s" % license["customer_phone"]
            print "customer_email = %s" % license["customer_email"]
            print "customer_description = %s" % license["customer_description"]
            print "max_created_vms = %d" % license["max_created_vms"]
            print "max_deployed_vms = %d" % license["max_deployed_vms"]
            print "max_images = %d" % license["max_images"]
            print "max_users = %d" % license["max_users"]
            print "max_clusters = %d" % license["max_clusters"]
            print "max_hosts = %d" % license["max_hosts"]
            print "effective_days = %d" % license["effective_days"]
            print "expired_date = %s" % license["expired_date"]
            
            print "hardware_fingerprints:"
            for hardware_fingerprint in license["hardware_fingerprints"]:
                print "    %s" % hardware_fingerprint
                
            print "special_cpus:"
            for special_cpu in license["special_cpus"]:
                print "    %s" % special_cpu