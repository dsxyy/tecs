#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：ip.py
# 测试内容：ip模块xmlrpc接口封装
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

class ip:
    ''' tecs ip methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session  
        
    def query(self,start_index,query_count,clustername,mac):
        result = self.apisvr.tecs.dhcp.ip.query(self.session,start_index,query_count,clustername,mac) 
        if result[0] != 0:
            print "ip:query ip information failed! error code =%d" % result[0]
            print "error string = %s" % result[1]
            return False
        else:
            print "ip information:" 
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            ipmac_list = result[3]
            print "---------ipmac list---------"  
            for ipmac in ipmac_list:
                print "cluster name= %s" % cluster["cluster_name"]
                for k in cluster.keys():
                    print "  %s = %s" % (k,cluster[k])
            return True
