#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
#
# 文件名称：user.py
# 测试内容：user模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：张文剑
# 完成日期：2012/03/24
#
# 修改记录1：
#    修改日期：2012/03/24
#    版 本 号：V1.0
#    修 改 人：张文剑
#    修改内容：创建
#*******************************************************************************/
import hashlib
from rpc import tecs_rpc

#****************************************************************************#
#                             常量定义                                       # 
#****************************************************************************#
START_INDEX = 0L
QUERY_COUNT = 200L
#用户角色
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

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    