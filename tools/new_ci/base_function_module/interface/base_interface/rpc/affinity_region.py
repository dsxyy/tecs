#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�affinity_region.py
# �������ݣ�affinity_regionģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ�
# ������ڣ�2013/08/30
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/08/30
#    �� �� �ţ�V1.0
#    �� �� �ˣ�
#    �޸����ݣ�����
#*******************************************************************************/
from rpc import tecs_rpc

#****************************************************************************#
#                             ��������                                       # 
#****************************************************************************#
START_INDEX = 0L
QUERY_COUNT = 200L
USER_CREAT_PROJECT = -5L
ALL_USER_PROJECT = -3L
QUERY_BY_PROJECT_NAME = 0L
NO_USER_NAME = ""
NO_PROJECT_NAME = ""
#****************************************************************************#

class affinity_region:
    ''' tecs project methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session
        
    def allocate(self,name,description,level,is_forced):
        result = self.apisvr.tecs.affinity_region.allocate(self.session,name,description,level,is_forced) 
        if result[0] != 0:
            print "failed to create affinity_region %s! error code = %d" % (name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to create affinity_region %s!" % name
            return True

    def delete(self,arid):
        result = self.apisvr.tecs.affinity_region.delete(self.session,arid) 
        if result[0] != 0:
            print "failed to delete affinity_region %s! error code = %d" % (arid,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to delete affinity_region %d!" % arid
            return True  

    def query(self,flag,arid,user_name):
        result = self.apisvr.tecs.affinity_region.query(self.session,START_INDEX,QUERY_COUNT,flag,arid,user_name) 
        if result[0] != 0:
            print "failed to query affinity_region %d! error code = %d" % (arid,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to query affinity_region %d!" % arid

        next_index = result[1]
        max_count = result[2]
        print 'query max count %d next index %d' % (max_count,next_index)

        if len(result[3]) == 0:
            print "no vm " 
            return True;

        ars  = result[3]
        for ar in ars:
            print "========affinity_region %d info==================="% ar["id"]
            print "user  = %s" % ar["user_name"]
            print "affinity_region name = %s" % ar["name"]
            print "description = %s" % ar["description"]
            print "level  = %s" % ar["level"]
            print "is_forced = %d" % ar["is_forced"]
            
        return True

    def set(self,arid,name,description,level,is_forced):
        result = self.apisvr.tecs.affinity_region.set(self.session,arid,name,description,level,is_forced) 
        if result[0] != 0:
            print "failed to set affinity_region %d! error code = %d" % (arid,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "success to set affinity_region %d!" % arid
            return True


