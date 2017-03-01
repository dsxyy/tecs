#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：netplane_default.py
# 测试内容：netplane_map模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张业兵
# 完成日期：2012/04/10
#
# 修改记录1：
#    修改日期：2012/04/10
#    版 本 号：V1.0
#    修 改 人：张业兵
#    修改内容：创建
# 修改记录1：
#    修改日期：2012/04/25
#    版 本 号：V1.0
#    修 改 人：陈志伟
#    修改内容：XML文件有误，根据实际情况修改了set 和query
#*******************************************************************************/
class netplane_default:
    ''' tecs netplane_default methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session

    def delete(self,cluster_name,if_name):
        result = self.apisvr.tecs.netplane_default.delete(self.session,cluster_name,if_name) 
        if result[0] != 0:
            print "fail to delete default netplane map of if_name %s in %s cluster! error code = %d" % \
                  (if_name,cluster_name,result[0])
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to delete default netplane map of if_name %s in %s cluster!" % \
                   (if_name,cluster_name)
            return True   

	'''这里查询出来的结构为 [_if_name,_netplane]数组, if_name暂时无用'''		
    def query(self,cluster_name,if_name):
        result = self.apisvr.tecs.netplane_default.query(self.session,cluster_name,if_name) 
        netplaneresult = " "
        if result[0] != 0:
            print "fail to query netplane of if_name %s in %s cluster! " % (if_name,cluster_name)
            print "error string = %s" % result[1]
            return None
        else:
            print "succeed to query netplane of if_name %s in %s cluster!" % (if_name,cluster_name) 
            if result[1] != "":
                netplaneinfo = result[1]
                for pt in netplaneinfo:
                    for n,v in pt.items():
                        print "  %-20s = %s" % (n,v)
                        if n == "netplane":
                            netplaneresult = v 
        return netplaneresult 
   
    def set(self,cluster_name,if_name,netplane):
        result = self.apisvr.tecs.netplane_default.set(self.session,cluster_name,if_name,netplane)
        if result[0] != 0:
            print "fail to set netplane!!!"   
            print "error string = %s" % result[1]
            return False
        else:
            print "succeed to set netplane !!!"
            return True

