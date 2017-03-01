#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：vlan.py
# 测试内容：vlan模块xmlrpc接口封装
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

class vlan:
    ''' tecs vlan methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session 
      
    def query(self,start_index,query_count,cluster_name,host_name):
        result = self.apisvr.tecs.vlan.query(self.session,start_index,query_count,cluster_name,host_name) 
        if result[0] != 0:
            print "fail to query  vlan of  %s host in %s cluster! error code =%d" % (host_name,cluster_name,result[0]) 
            return False
        else:
            print "succeed to query  vlan of  %s host in %s cluster success!" % (host_name,cluster_name) 
         
        print "net plane map:" 
        print "next_index = %d" % result[1]
        print "max_count = %d" % result[2]
        print "netplane_name = %d" % result[3]

        vlans = result[4]
        for vn in vlans:
            for n,v in vn.items(): 
                print "%-15s = %s" % (n,v )

        return True  

