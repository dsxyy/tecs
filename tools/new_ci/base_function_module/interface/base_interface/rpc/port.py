#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：port.py
# 测试内容：port模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张业兵
# 完成日期：2012/04/10
#
# 修改记录1：
#    修改日期：2012/04/10
#    版 本 号：V1.0
#    修 改 人：张业兵
#    修改内容：创建
#*******************************************************************************/
#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
NO_PORT_NAME = ""
#端口使用情况 0-状态未知 1-使用
PORT_USED_UNKNOWN = 0
PORT_USED_YES = 1

#****************************************************************************#
from rpc import tecs_rpc

class port:
    ''' tecs port methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session

    def delete(self,cluster_name,host_name,port_name):
        result = self.apisvr.tecs.port.delete(self.session,cluster_name,host_name,port_name) 
        if result[0] != 0:
            print "fail to delete %s port of %s host in %s cluster! error code = %d" % (port_name,host_name,cluster_name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to delete %s port of %s host in %s cluster!" % (port_name,host_name,cluster_name)
            return True   
      
    def query(self,cluster_name,host_name,port_name):
        result = self.apisvr.tecs.port.query(self.session,cluster_name,host_name,port_name) 
        if result[0] != 0:
            print "fail to query  %s port of  %s host in %s cluster! error code =%d" % (port_name,host_name,cluster_name,result[0])
            print "error string = %s" % result[1] 
            return None
        else:
            print "succeed to query  %s port of  %s host in %s cluster success!" % (port_name,host_name,cluster_name) 
         
        ports = result[1]
        for pt in ports:
            for n,v in pt.items():
                print "  %-20s = %s" % (n,v)
        return ports 
   
    def set(self,cluster_name,host_name,port_name,port_paras):
        result = self.apisvr.tecs.port.set(self.session,cluster_name,host_name,port_name,port_paras)
        if result[0] != 0:
            print "fail to set %s port of %s host in %s cluster! error code = %d" % (port_name,host_name,cluster_name,result[0])     
            print "parameter:"
            for n,v in port_paras.items():                
                print n,v
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to set %s port of %s host in %s cluster!" % (port_name,host_name,cluster_name)
            return True

    def query_count(self,cluster_name,host_name,port_name):
        result = self.apisvr.tecs.port.query(self.session,cluster_name,host_name,port_name) 
        if result[0] != 0:
            print "fail to query  %s port of  %s host in %s cluster! error code =%d" % (port_name,host_name,cluster_name,result[0])
            print "error string = %s" % result[1] 
            return None
        else:
            print "succeed to query  %s port of  %s host in %s cluster success!" % (port_name,host_name,cluster_name) 
         
        ports = result[1]
        count = 0
        for pt in ports:
            count = count+1
            for n,v in pt.items():
                print "  %-20s = %s" % (n,v)
        return count 

    def query_value(self,cluster_name,host_name,port_name,query_name):
        result = self.apisvr.tecs.port.query(self.session,cluster_name,host_name,port_name) 
        if result[0] != 0:
            print "fail to query  %s port of  %s host in %s cluster! error code =%d" % (port_name,host_name,cluster_name,result[0])
            print "error string = %s" % result[1] 
            return None
        else:
            print "succeed to query  %s port of  %s host in %s cluster success!" % (port_name,host_name,cluster_name) 
         
        ports = result[1]
        count = 0
        for pt in ports:
            count = count+1
            for n,v in pt.items():
                print "  %-20s = %s" % (n,v)
        for pt in ports:
            return pt[query_name]
        return None

    def query_use(self,cluster_name,host_name,port_name):
        return self.query_value(cluster_name,host_name,port_name,"used") 
