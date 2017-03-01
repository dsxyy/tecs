#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
#
# �ļ����ƣ�user.py
# �������ݣ�userģ��xmlrpc�ӿڷ�װ
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
import hashlib
from rpc import tecs_rpc

#****************************************************************************#
#                             ��������                                       # 
#****************************************************************************#
START_INDEX = 0L
QUERY_COUNT = 200L
#�û���ɫ
CLOUDADMIN = 1
ACCOUNTADMIN = 2
ACCOUNTUSER = 3
USER_ENABLE = 1
USER_DISABLE = 2
NO_DESCRIPTION = ""
NO_PHONE = ""
NO_EMAIL = ""
NO_ADDR = ""
#****************************************************************************#

class file_manager:
    ''' tecs user methods '''
    def __init__(self, host, session):
        self.api = tecs_rpc(host)
        self.apisvr = self.api.server
        self.session = session


    def query(self):
        result = self.apisvr.tecs.file.query_coredump_url(self.session)
        if result[0] != 0:
            print "error string = %s" % result[1]
            return False
        else:
            print "success to query file manager url"
            file_manager_url = result[1]
            print "---------show url---------"
            for k in file_manager_url.keys():
                print "  %s = %s" % (k,file_manager_url[k])
            return True

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    