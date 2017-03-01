#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：netplane.py
# 测试内容：netplane模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：wh
# 完成日期：2012/11/1
#
#*******************************************************************************/
from rpc import tecs_rpc

class netplane:
    ''' tecs netplane methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session

		
    def add(self,cluster_name,netplane,description):
        result = self.apisvr.tecs.netplane.add(self.session,cluster_name,netplane,description)
        if result[0] != 0:
            print "fail to add netplane!!!"   
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to add netplane !!!"
            return True
			
			
			
    def delete(self,cluster_name,netplane):
        result = self.apisvr.tecs.netplane.delete(self.session,cluster_name,netplane) 
        if result[0] != 0:
            print "fail to delete netplane map in %s cluster! error code = %d" % (cluster_name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to delete netplane map in %s cluster  !" % (cluster_name)
            return True   


		
    def set(self,cluster_name,netplane,description):
        result = self.apisvr.tecs.netplane_default.set(self.session,cluster_name,netplane,description)
        if result[0] != 0:
            print "fail to set netplane!!!"   
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to set netplane !!!"
            return True

