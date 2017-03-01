#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：trunk.py
# 测试内容：trunk模块xmlrpc接口封装
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
from rpc import tecs_rpc

class trunk:
    ''' tecs trunk methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session

    def delete(self,cluster_name,host_name,trunk_name,trunk_ports):
        result = self.apisvr.tecs.trunk.delete(self.session,cluster_name,host_name,trunk_name,trunk_ports)
        for port in  trunk_ports:
            print "del port %s" % port

        if result[0] != 0:
            print "fail to delete port of %s trunk on %s host in %s cluster! error code = %d" % (trunk_name,host_name,cluster_name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to delete port of %s trunk on %s host in %s cluster!" % (trunk_name,host_name,cluster_name)
            return True   
      
    def query(self,cluster_name,host_name,trunk_name):
        result = self.apisvr.tecs.trunk.query(self.session,cluster_name,host_name,trunk_name) 
        if result[0] != 0:
            print "fail to query  %s trunk on %s host in %s cluster ! error code =%d" % (trunk_name,host_name,cluster_name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to query  %s trunk on %s host in %s cluster!" % (trunk_name,host_name,cluster_name)
        
        trunks = result[1]
        for tk in  trunks:
            for n ,v in tk.items():
                if isinstance(v,list):
                    print "%-15s = %s" % (n,' '.join(v))
                else:
                    print "%-15s = %s" % (n,v)
        return True 
   
    def set(self,cluster_name,host_name,trunk_name,trunk_type,netplane,trunk_ports):       
        result = self.apisvr.tecs.trunk.set(self.session,cluster_name,host_name,trunk_name,trunk_type,netplane,trunk_ports)

        def tprint(cluster_name,host_name,trunk_name,trunk_type,trunk_ports):
            print "trunk parameters:"
            print "%-15s = %s" % ("cluster_name",cluster_name)
            print "%-15s = %s" % ("host_name",host_name)
            print "%-15s = %s" % ("trunk_name",trunk_name)
            print "%-15s = %s" % ("trunk_type",trunk_type)
            print "%-15s = %s" % ("netplane",netplane)
            print "%-15s = %s" % ("trunk_ports",' '.join(trunk_ports))

        if result[0] != 0:
            print "fail to set trunk! error code = %d" % (result[0])
            tprint(cluster_name,host_name,trunk_name,trunk_type,netplane,trunk_ports)
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to trunk!"
            tprint(cluster_name,host_name,trunk_name,trunk_type,netplane,trunk_ports)
            return True

