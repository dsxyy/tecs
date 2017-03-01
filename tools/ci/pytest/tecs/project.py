#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：project.py
# 测试内容：project模块xmlrpc接口封装
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
USER_CREAT_PROJECT = -5L
APPOINTED_USER_PROJECT = -4L
ALL_USER_PROJECT = -3L
QUERY_BY_PROJECT_NAME = 0L
NO_USER_NAME = ""
NO_PROJECT_NAME = ""
#****************************************************************************#

class project:
    ''' tecs project methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session
        
    def action(self,name,operate):
        result = self.apisvr.tecs.project.action(self.session,name,operate) 
        if result[0] != 0:
            print "failed to query project %s! error code = %d" % (name,result[0])
            print "error string = %s" % result[1]
            return None,None
        else:
            print "success to %s project !" % operate

        workflow_id = result[1]
        target = result[2]
       
        print '%s target %s  id %s ' % (operate,target,workflow_id)
        return target,workflow_id 
        
    def allocate(self,name,description):
        result = self.apisvr.tecs.project.allocate(self.session,name,description) 
        if result[0] != 0:
            print "failed to create project %s! error code = %d" % (name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create project %s!" % name
            return True

    def delete(self,name,user_name):
        result = self.apisvr.tecs.project.delete(self.session,name,user_name) 
        if result[0] != 0:
            print "failed to delete project %s! error code = %d" % (name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete project %s!" % name
            return True  

    def query(self,start_index,query_count,flag,name,user_name):
        result = self.apisvr.tecs.project.query(self.session,start_index,query_count,flag,name,user_name) 
        if result[0] != 0:
            print "failed to query project %s! error code = %d" % (name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to query project %s!" % name

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)
        pjs  = result[3]
        for pj in pjs:
            for n,v in pj.items():
                print '%s = %s' % (n,v)
        return True

    def query_vm_by_project(self,start_index,query_count,name,user_name):
        result = self.apisvr.tecs.project.query_vm_by_project(self.session,start_index,query_count,name,user_name) 
        if result[0] != 0:
            print "failed to query vm from %d to %d by project %s! error code = %d" % (start_index,start_index+query_count,name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to query vm by project %s!" % name

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)
        vms  = result[3]
        for vm in vms:
            for n,v in vm.items():
                print '%s = %s' % (n,v)
        return True

    def set(self,name,description):
        result = self.apisvr.tecs.project.set(self.session,name,description) 
        if result[0] != 0:
            print "failed to set project %s! error code = %d" % (name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set project %s!" % name
            return True
    def query_vm_cout_by_project(self,start_index,query_count,name,user_name):
        result = self.apisvr.tecs.project.query_vm_by_project(self.session,start_index,query_count,name,user_name) 
        if result[0] != 0:
            print "failed to query vm from %d to %d by project %s! error code = %d" % (start_index,start_index+query_count,name,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to query vm by project %s!" % name

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)
        vms  = result[3]
        for vm in vms:
            for n,v in vm.items():
                print '%s = %s' % (n,v)
        return max_count


    def query_vm_value_by_project(self,start_index,query_count,name,user_name,query_name):
        result = self.apisvr.tecs.project.query_vm_by_project(self.session,start_index,query_count,name,user_name) 
        if result[0] != 0:
            print "failed to query vm from %d to %d by project %s! error code = %d" % (start_index,start_index+query_count,name,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to query vm by project %s!" % name

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)
        vms  = result[3]
        for vm in vms:
            for n,v in vm.items():
                print '%s = %s' % (n,v)
        for vm in vms:
            return vm[query_name] 
        return None

    def query_vm_name_by_project(self,start_index,query_count,name,user_name):
        return self.query_vm_value_by_project(start_index,query_count,name,user_name,"vm_name") 

    def query_value(self,start_index,query_count,flag,name,user_name,query_name):
        result = self.apisvr.tecs.project.query(self.session,start_index,query_count,flag,name,user_name) 
        if result[0] != 0:
            print "failed to query project %s! error code = %d" % (name,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to query project %s!" % name

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)
        pjs  = result[3]
        for pj in pjs:
            for n,v in pj.items():
                print '%s = %s' % (n,v)

        for pj in pjs:
            return pj[query_name]
        return None

    def query_count(self,start_index,query_count,flag,name,user_name):
        result = self.apisvr.tecs.project.query(self.session,start_index,query_count,flag,name,user_name) 
        if result[0] != 0:
            print "failed to query project %s! error code = %d" % (name,result[0])
            print "error string = %s" % result[1]
            return None
        else:
            print "success to query project %s!" % name

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)
        pjs  = result[3]
        for pj in pjs:
            for n,v in pj.items():
                print '%s = %s' % (n,v)

        return max_count

    def query_des_by_name(self,name):
        return self.query_value(START_INDEX,QUERY_COUNT,QUERY_BY_PROJECT_NAME,name,NO_USER_NAME,"des")

    def query_des_by_user_name(self,user_name):
        return self.query_value(START_INDEX,QUERY_COUNT,USER_CREAT_PROJECT,NO_PROJECT_NAME,user_name,"des")

    def query_des_by_current_user_name(self):
        return self.query_value(START_INDEX,QUERY_COUNT,USER_CREAT_PROJECT,NO_PROJECT_NAME,NO_USER_NAME,"des")


    def statistics_static_data(self,start_index,query_count,flag,name,user_name):
        result = self.apisvr.tecs.project.statistics.static(self.session,start_index,query_count,flag,name,user_name) 
        if result[0] != 0:
            print "failed to query statistics_static_data from %d to %d by project %s! error code = %d" % (start_index,start_index+query_count,name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to query statistics_static_data by project %s!" % name

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)

        if max_count == 0:
            return True
        else:    
            pjs  = result[3]
            for pj in pjs:
                for n,v in pj.items():
                    print '%s = %s' % (n,v)
            return True
		