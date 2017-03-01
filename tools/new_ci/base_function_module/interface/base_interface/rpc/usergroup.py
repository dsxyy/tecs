#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：usergroup.py
# 测试内容：usergroup模块xmlrpc接口封装
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
#用户组类型暂未使用，无意义
GROUP_TYPE1 = 1
GROUP_TYPE2 = 2
UG_ENABLE = 1
UG_DISABLE = 2

#****************************************************************************#
class usergroup:
    ''' tecs usergroup methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session

    def allocate(self,groupname,grouptype):
        result = self.apisvr.tecs.usergroup.allocate(self.session,groupname,grouptype) 
        if result[0] != 0:
            print "failed to create user group %s! error code = %d" % (groupname,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create user group %s!" % groupname
            return True
            
    def delete(self,groupname):
        result = self.apisvr.tecs.usergroup.delete(self.session,groupname) 
        if result[0] != 0:
            print "failed to delete user group %s! error code = %d" % (groupname,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete user group %s!" % groupname
            return True   

    def query(self,start_index,query_count,groupname):
        result = self.apisvr.tecs.usergroup.query(self.session,start_index,query_count,groupname) 
        if result[0] != 0:
            print "failed to query user group %s! error code = %d" % (groupname,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to query user group %s!" % groupname 
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            user_list = result[3]
            print "---------user list---------"  
            for usergroup in user_list:
                print "usergroup %s" % groupname
                for k in usergroup.keys():
                    print "  %s = %s" % (k,usergroup[k])
            return True    
            
    def set(self,groupname,enabled,description):
        result = self.apisvr.tecs.usergroup.set(self.session,groupname,enabled,description) 
        if result[0] != 0:
            print "failed to set user group: %s! error code = %d" % (groupname,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set user group: %s!" % groupname
            return True   

    def get_use_flag(self,groupname):
        result = self.apisvr.tecs.usergroup.query(self.session,START_INDEX,QUERY_COUNT,groupname) 
        if result[0] != 0:
            print "failed to get user group: %s useflag! error code = %d" % (groupname,result[0])
            print "error string = %s" % result[1]
            return -1
        else:
            print "success to get user group: %s useflag!" % groupname
            user_list = result[3]
            for usergroup in user_list:
                return usergroup["enabled"]

            return -1   
           