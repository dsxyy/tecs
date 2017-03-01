#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�ip.py
# �������ݣ�ipģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ���ѩ��
# ������ڣ�2012/04/12
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/04/12
#    �� �� �ţ�V1.0
#    �� �� �ˣ���ѩ��
#    �޸����ݣ�����
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
