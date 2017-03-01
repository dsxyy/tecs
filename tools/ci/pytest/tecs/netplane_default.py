#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�netplane_default.py
# �������ݣ�netplane_mapģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ���ҵ��
# ������ڣ�2012/04/10
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/04/10
#    �� �� �ţ�V1.0
#    �� �� �ˣ���ҵ��
#    �޸����ݣ�����
# �޸ļ�¼1��
#    �޸����ڣ�2012/04/25
#    �� �� �ţ�V1.0
#    �� �� �ˣ���־ΰ
#    �޸����ݣ�XML�ļ����󣬸���ʵ������޸���set ��query
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

	'''�����ѯ�����ĽṹΪ [_if_name,_netplane]����, if_name��ʱ����'''		
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

