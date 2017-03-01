#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�vlan.py
# �������ݣ�vlanģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ���ҵ��
# ������ڣ�2012/04/10
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/04/10
#    �� �� �ţ�V1.0
#    �� �� �ˣ���ҵ��
#    �޸����ݣ�����
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

