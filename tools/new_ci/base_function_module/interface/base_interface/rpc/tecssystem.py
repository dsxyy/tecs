#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�system.py
# �������ݣ�system����ʱ��Ϣ��ѯ����ģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/07/07
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/07/07
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
#*******************************************************************************/
from rpc import tecs_rpc

#****************************************************************************#
#                             ��������                                       # 
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
        '''��ѯָ���ڵ������ʱ��Ϣ'''
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
        '''��ȡָ���ڵ������ʱ״̬'''
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
        '''��ѯ���֤��Ϣ'''
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