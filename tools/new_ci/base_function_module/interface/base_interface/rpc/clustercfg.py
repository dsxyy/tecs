#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：clustercfg.py
# 测试内容：clustercfg模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：刘雪峰
# 完成日期：2012/04/12
#
# 修改记录1：
#    修改日期：2012/04/12
#    版 本 号：V1.0
#    修 改 人：刘雪峰
#    修改内容：创建
#*******************************************************************************/
from rpc import tecs_rpc

class clustercfg:
    ''' tecs clustercfg methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        
    def delete(self,clustername,configname):
        result = self.apisvr.tecs.clustercfg.delete(self.session,clustername,configname) 
        if result[0] == 0:
            print "clustercfg:delete clustername %s configname %s success!" % (clustername,configname)
            return True
        else:
            print "failed! error code = %s" % result[0]
            print "error string = %s" % result[1]
            return False   
    
    def query(self,clustername,configname):
        result = self.apisvr.tecs.clustercfg.query(self.session,clustername,configname) 
        if result[0] != 0:
            print "clustercfg:query clustercfg information failed! error code =%d" % result[0] 
            return False
        else:
            print "-------clustercfg information-------:" 
            print "cluster_name %s" % clustername
            clustercfg_list = result[1]
            for clustercfg in clustercfg_list:
                for k in clustercfg.keys():
                    print "  %s = %s" % (k,clustercfg[k])
            return True

    def query_config_value(self,clustername,configname):
        result = self.apisvr.tecs.clustercfg.query(self.session,clustername,configname) 
        if result[0] != 0:
            print "clustercfg:query cluster config value failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return None
        else:
            print configname
            clustercfg_list = result[1]
            if configname is None:
                return clustercfg_list
            else:
                for clustercfg in clustercfg_list:
                    for n,v in clustercfg.items():
                        if n == "config_value":
                            return  v
            
            return None
            
   