#!/usr/bin/python
# -*- coding: gb2312 -*-
#*******************************************************************************
# Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
# 
# 文件名称：image.py
# 测试内容：image模块xmlrpc接口封装
# 当前版本：1.0 
# 作    者：李孝成
# 完成日期：2012/03/24
#*******************************************************************************/
from contrib import xmlrpclibex
from contrib import tecs_common

# RPC配置
RPC_PORT="8080"
RPC_ROOT_NAME="admin"
RPC_ROOT_PWD="admin"

class tecs_rpc:
    ''' tecs image methods '''
    def __init__(self, server_addr):
        server_url = '%s%s%s%s%s%s%s%s%s' % ('http://',RPC_ROOT_NAME,':',RPC_ROOT_PWD,'@',server_addr,':',RPC_PORT,'/RPC2')
        print "Connect to xmlrpc url %s ..." % server_url
        try:
            self.server = xmlrpclibex.ServerProxy(server_url)
        except:
            ci_exit(-1,"failed to create xmlrpc ServerProxy!")
    
        
        
