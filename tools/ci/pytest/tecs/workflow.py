#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�port.py
# �������ݣ�portģ��xmlrpc�ӿڷ�װ
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
#****************************************************************************#
#                             ��������                                       # 
#****************************************************************************#
NO_PORT_NAME = ""
#�˿�ʹ����� 0-״̬δ֪ 1-ʹ��
PORT_USED_UNKNOWN = 0
PORT_USED_YES = 1

#****************************************************************************#

class worflow:
    ''' tecs worflow methods '''
    def __init__(self, server,session):
        self.apisvr = server
        self.session = session
      
    def query(self,workflow_addr):
        result = self.apisvr.tecs.workflow.query(self.session,workflow_addr) 
        if result[0] != 0:
            print "fail to query  %s" % (workflow_addr)
            print "error string = %s" % result[1] 
            return None
        workflow_states =  result[1]["workflow_states"]
        print "'work state %s ' " % (result[1])
        #for ws in workflow_states:
           #print "'work state %s ' " % (ws)
        return workflow_states 
   
