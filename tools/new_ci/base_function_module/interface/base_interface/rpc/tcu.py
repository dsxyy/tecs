#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�tcu.py
# �������ݣ�tcuģ��xmlrpc�ӿڷ�װ
# ��ǰ�汾��1.0 
# ��    �ߣ����Ľ�
# ������ڣ�2012/03/24
#
# �޸ļ�¼1��
#    �޸����ڣ�2012/03/24
#    �� �� �ţ�V1.0
#    �� �� �ˣ����Ľ�
#    �޸����ݣ�����
#*******************************************************************************/
from rpc import tecs_rpc

#****************************************************************************#
#                             ��������                                       # 
#****************************************************************************#
TCU_NUM2 = 4
TCU_NUM4 = 4
TCU_NUM8 = 8
NO_CPU_INFO = ""
CPU_INFO_ID = 100L
#****************************************************************************#

class tcu:
    ''' tecs tcu methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session
           
    def set(self,cluster_name,cpu_info_id,tcu_num,description):
        result = self.apisvr.tecs.tcu.set(self.session,cluster_name,cpu_info_id,tcu_num,description) 
        if result[0] == 0:
            print "set tcu(cpu_info_id=%d,tcu_num=%d) in cluster %s success!" % (cpu_info_id,tcu_num,cluster_name)
            return True
        else:
            print "failed to set tcu(cpu_info_id=%d,tcu_num=%d) in cluster %s! error code = %s" % (cpu_info_id,tcu_num,cluster_name,result[0])
            print "error string = %s" % result[1]
            return False
    
    def delete(self,cluster_name,cpu_info_id):
        result = self.apisvr.tecs.tcu.delete(self.session,cluster_name,cpu_info_id) 
        if result[0] == 0:
            print "delete tcu(cpu_info_id=%d) in cluster %s success!" % (cpu_info_id,cluster_name)
            return True
        else:
            print "failed to set tcu(cpu_info_id=%d) in cluster %s! error code = %s" % (cpu_info_id,cluster_name,result[0])
            print "error string = %s" % result[1]
            return False

    def query(self,start_index,query_count,cluster_name,cpu_info,cpu_info_id):
        result = self.apisvr.tecs.tcu.query(self.session,start_index,query_count,cluster_name,cpu_info,cpu_info_id) 
        if result[0] != 0:
            print "failed to query tcu information in cluster %s! error code =%d" % (cluster_name,result[0]) 
            print "error string = %s" % result[1]
            return False
        else:
            print "cluster %s tcu information:" % cluster_name
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            tcu_list = result[3]
            print "tcu list:" 
            for s in tcu_list:
                for n,v in s.items():
                    print "%-20s = %s" % (n,v)
                #print "cpu_info = %s" % s["cpu_info"]
                #print "cpu_info_id = %s" % s["cpu_info_id"]
                #print "tcu_num = %s" % s["tcu_num"]
                #print "description = %s" % s["description"]
            return True  

    def get_key_value_by_id(self,cluster_name,cpu_info,cpu_info_id,key):
        result = self.apisvr.tecs.tcu.query(self.session,0L,200L,cluster_name,cpu_info,cpu_info_id) 
        if result[0] != 0:
            print "failed to query tcu information in cluster %s! error code =%d" % (cluster_name,result[0]) 
            print "error string = %s" % result[1]
            return None
        else:
            print "cluster %s tcu information:" % cluster_name
            print "next_index = %d" % result[1]
            print "max_count = %d" % result[2]
            tcu_list = result[3]
            print "tcu list:" 
            for s in tcu_list:
                for n,v in s.items():
                    print "%-20s = %s" % (n,v)               
                #print "cpu_info = %s" % s["cpu_info"]
                #print "cpu_info_id = %s" % s["cpu_info_id"]
                #print "tcu_num = %s" % s["tcu_num"]
                #print "description = %s" % s["description"]
                return s[key]
            return None  

    def get_tcu_num_by_id(self,cluster_name,cpu_info,cpu_info_id):
        return self.get_key_value_by_id(cluster_name,cpu_info,cpu_info_id,"tcu_num") 
    
    def get_description_by_id(self,cluster_name,cpu_info,cpu_info_id):
        return self.get_key_value_by_id(cluster_name,cpu_info,cpu_info_id,"description")